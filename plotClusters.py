from EventStore import EventStore
from ROOT import gSystem, TFile, TH2D, TCanvas,TPaveText

store = EventStore("output_ecalReco_test.root")
hECal = TH2D('hECal',"ECAl map",359,-179,179,315,-157,157)
hECalTrue = TH2D('hECalTrue',"ECal map",359,-179,179,315,-157,157)
for iev, event in enumerate(store):
    entries = event.get("caloClusters")
    for ipos, pos in enumerate(entries):
        hECal.Fill(pos.core().position.x, pos.core().position.y, pos.core().position.z)
    hits = event.get("caloCellsPositions")
    for ipos, pos in enumerate(hits):
        hECalTrue.Fill(pos.position().x, pos.position().y, pos.position().z)


canv = TCanvas( 'canv', 'ECal', 1200, 800 )
pad = canv.cd(1)
pad.SetLogz()
hECal.Draw('colz')
hECal.GetXaxis().SetRangeUser(-20,20)
hECal.GetYaxis().SetRangeUser(-50,-20)
print("Integral: ",hECal.Integral()/1000, "GeV")
pt1 = TPaveText(0.1,0.8,0.5,0.9,"brNDC")
pt1.SetFillColorAlpha(0,1)
pt1.AddText("#color["+str(4)+"]{energy: "+str(int(hECal.Integral()/1000))+" GeV}")
pt1.Draw()
canv.Update()
canv.Print("Ecal_map.png")
