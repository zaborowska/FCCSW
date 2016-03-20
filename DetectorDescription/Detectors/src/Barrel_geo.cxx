//
//  Barrel_geo.cxx
//
//
//  Created by Julia Hrdinka on 15/12/14.
//
//

#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/TGeoUnits.h"
#include "DetExtensions/DetCylinderLayer.h"
#include "DetExtensions/DetModule.h"
#include "DetExtensions/DetExtension.h"
#include "DetExtensions/DetCylinderVolume.h"
#include "DetExtensions/DetSensComponent.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens)
{

    xml_det_t x_det = e;
    string det_name = x_det.nameStr();
    Material air = lcdd.air();

    //Detector envelope of subdetector
    DetElement tracker(det_name, x_det.id());
    //get status for the RecoGeometry
    xml_comp_t x_status = x_det.child(_U(status));
    int status     = x_status.id();
    //add Extension to Detlement for the RecoGeometry
    Det::DetCylinderVolume* detvolume = new Det::DetCylinderVolume(status);
    tracker.addExtension<Det::IDetExtension>(detvolume);
    //Create the Volume of the Detector envelope
    DD4hep::XML::Dimension x_det_dim(x_det.dimensions());
    double z = x_det_dim.z();
    Tube tracker_shape(x_det_dim.rmin(),x_det_dim.rmax(),z);
    Volume tracker_vol(x_det.nameStr()+"_envelope",tracker_shape, air);
    //Vizualization
    tracker_vol.setVisAttributes(lcdd.invisible());
//    tracker_vol.setVisAttributes(lcdd, x_det.visStr());
    //Set sensitive type tracker
    sens.setType("Geant4Tracker");

    int layer_num = 0;

    //Go through layers
    for (xml_coll_t j(e,_U(layer)); j; ++j )
    {
        xml_comp_t x_layer = j;
        double rmin     = x_layer.inner_r();
        double rmax     = x_layer.outer_r();
        double radius   = (rmax+rmin)*0.5;
        double layer_z  = x_layer.z();
        double dr       = x_layer.dr();

        //Create Volume and DetElement for Layer
        string layer_name  = det_name + _toString(layer_num,"layer%d");
        Volume layer_vol(layer_name,Tube(rmin,rmax,layer_z), lcdd.material(x_layer.materialStr()));
        DetElement lay_det (tracker,layer_name,layer_num);

        //Visualization
        layer_vol.setVisAttributes(lcdd.invisible());

        //module in phi // later also loop through modules for different modules
        xml_comp_t x_module = x_layer.child(_U(module));
        int repeat = x_module.repeat();
        double deltaphi = 2.*M_PI/repeat;
        //slices in z
        xml_comp_t x_slice = x_layer.child(_U(slice));
        int zrepeat = x_slice.repeat();
        double dz       = x_slice.z();
        //add Extension to Detlement for the RecoGeometry
        Det::DetCylinderLayer* detcylinderlayer = new Det::DetCylinderLayer();
        lay_det.addExtension<Det::IDetExtension>(detcylinderlayer);

        int module_num = 0;
        //Place the Modules in z
        for (int k = -zrepeat; k<=zrepeat; k++)
        {
            double r = radius;
            string zname = _toString(k,"z%d");
            if (k % 2==0) {
                r += dr;
            }
            //Place the Modules in phi
            for (int i = 0; i < repeat; ++i)
            {
                //Create Module Volume
                Volume mod_vol("module", Box(x_module.length(),x_module.width(),x_module.thickness()), lcdd.material(x_module.materialStr()));
                //Visualization
                mod_vol.setVisAttributes(lcdd.invisible());

                double phi = deltaphi/dd4hep::rad * i;
                string module_name = zname + _toString(i,"module%d");

                Position trans(r*cos(phi),
                               r*sin(phi),
                               k*dz);

                //Create module Detelement
                DetElement mod_det(lay_det,module_name,module_num);
                //add Extension to Detlement for the RecoGeometry
                Det::DetModule* detmod = new Det::DetModule();
                mod_det.addExtension<Det::IDetExtension> (detmod);

                int comp_num = 0;
                //go through module components
                for (xml_coll_t n(x_module,_U(module_component)); n; ++n) {
                    xml_comp_t x_comp = n;
                    Volume comp_vol(_toString(comp_num, "component% ") + x_comp.materialStr(), Box(x_comp.length(),x_comp.width(), x_comp.thickness()),lcdd.material(x_comp.materialStr()));
                    //Visualization
                    comp_vol.setVisAttributes(lcdd, x_comp.visStr());
                    //Create DetElement
                    DetElement comp_det(mod_det, "component, " + x_comp.materialStr(),comp_num);
                    //Set Sensitive Volmes sensitive
                    if (x_comp.isSensitive()) {
                        comp_vol.setSensitiveDetector(sens);
                        //add Extension for sensitive component
                        Segmentation segmentation(sens.readout().segmentation());
                        Det::DetSensComponent* ex = new Det::DetSensComponent(segmentation);
                        comp_det.addExtension<Det::IDetExtension> (ex);
                    }
                    else {
                        //add Extension
                        Det::DetExtension* ex = new Det::DetExtension();
                        comp_det.addExtension<Det::IDetExtension> (ex);
                    }
                    //place component in module
                    Position transComp (0.,0.,x_comp.z());
                    PlacedVolume placedcomp = mod_vol.placeVolume(comp_vol,transComp);
                    //assign the placed Volume to the DetElement
                    comp_det.setPlacement(placedcomp);
                    placedcomp.addPhysVolID("component",comp_num);
                    ++comp_num;
                }

                //Place Module Box Volumes in layer
                PlacedVolume placedmodule = layer_vol.placeVolume(mod_vol, Transform3D(RotationX(-0.5*M_PI)*RotationZ(-0.5*M_PI)*RotationX(phi-0.6*M_PI),trans));
                placedmodule.addPhysVolID("module", module_num);
                // assign module DetElement to the placed Module volume
                mod_det.setPlacement(placedmodule);


                ++module_num;
            }
            ++module_num;
        }
        //Place Layervolume

        PlacedVolume placedLayer = tracker_vol.placeVolume(layer_vol);
        placedLayer.addPhysVolID("layer",layer_num);
        //Assign Layer DetElement to LayerVolume
        lay_det.setPlacement(placedLayer);
        ++layer_num;

    }
    Volume mother_vol = lcdd.pickMotherVolume(tracker);
    //Place envelopevolume in mothervolume
    PlacedVolume placed_env = mother_vol.placeVolume(tracker_vol);
    placed_env.addPhysVolID("system",x_det.id());
    //assign tracker DetElement to tracker volume
    tracker.setPlacement(placed_env); //fuer envelope moeglich


    return tracker;
}

DECLARE_DETELEMENT( Barrel, create_element )
