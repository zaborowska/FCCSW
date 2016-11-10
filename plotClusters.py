from EventStore import EventStore
from ROOT import gSystem, TFile, TH2D, TCanvas

store = EventStore("output_ecalReco_test.root")
hECal = TH2D('hECal',"ECAl map",359,-179,179,315,-157,157)
hECalTrue = TH2D('hECalTrue',"ECAl map",359,-179,179,315,-157,157)
for iev, event in enumerate(store):
    entries = event.get("caloClusters")
    for ipos, pos in enumerate(entries):
        hECal.Fill(pos.core().position.x, pos.core().position.y, pos.core().position.z)
    hits = event.get("caloCellsPositions")
    for ipos, pos in enumerate(hits):
        hECalTrue.Fill(pos.position().x, pos.position().y, pos.position().z)


canv = TCanvas( 'canv', 'ECal', 1500, 800 )
canv.cd(1)
hECal.Draw('colz')
hECal.GetXaxis().SetRangeUser(-20,20)
hECal.GetYaxis().SetRangeUser(-50,-20)
canv.Update()
canv.Print("Ecal_map.png")
