from EventStore import EventStore
import sys
from math import ceil
from ROOT import gSystem, TFile, TH2D, TCanvas,TPaveText, TF2, gStyle

store = EventStore("output_ecalReco_test.root")
hECal = TH2D('hECal',"ECAl map",359,0,359,1257,0,1257)
for iev, event in enumerate(store):
    entries = event.get("caloClusters")
    for ipos, pos in enumerate(entries):
        hECal.Fill(pos.core().position.x, pos.core().position.y, pos.core().position.z)


canv = TCanvas( 'canv', 'ECal', 1200, 800 )
pad = canv.cd(1)
# pad.SetLogz()
hECal.Draw('colz')
# f2dgaus = TF2("f2dgaus","[0]*TMath::Gaus(x,[1],[2])*TMath::Gaus(y,[3],[4])",0,10,0,10)
# f2dgaus.SetParameters(100,180,1,592,1)
# hECal.Fit(f2dgaus)
hECal.GetXaxis().SetRangeUser(160,195)
hECal.GetYaxis().SetRangeUser(580,605)
canv.SetGrid()
hECal.GetXaxis().SetLabelSize(0.02)
hECal.GetYaxis().SetLabelSize(0.02)
hECal.GetXaxis().SetNdivisions(35)
hECal.GetYaxis().SetNdivisions(25)

sumEn = 0
for ix in range (179,182):
    for iy in range (592,595):
        print (ix,iy)
        sumEn += hECal.GetBinContent(ix,iy)
print("Energy in 3x3 centre  ",sumEn/hECal.Integral())
print("Total energy: ",hECal.Integral(), "GeV")
pt1 = TPaveText(0.1,0.8,0.5,0.9,"brNDC")
pt1.SetFillColorAlpha(0,1)
pt1.AddText("#color["+str(4)+"]{energy: "+str(int(ceil(hECal.Integral())))+" GeV}")
# pt1.AddText("#color["+str(4)+"]{en in 3x3: "+str(int(sumEn/hECal.Integral()))+" %}")
pt1.Draw()
canv.Update()
if(len(sys.argv) > 1):
    canv.Print("Ecal_map_"+sys.argv[1]+".png")
    canv.Print("Ecal_map_"+sys.argv[1]+".root")
else:
    canv.Print("Ecal_map.png")
