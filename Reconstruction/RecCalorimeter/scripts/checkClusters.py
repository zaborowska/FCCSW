from ROOT import gSystem
from EventStore import EventStore

gSystem.Load("libdatamodelDict")
store = EventStore(["./output_ecalReco_test.root"])

for iev, event in enumerate(store):
    clusters = event.get('caloClusters')
    assert(len(clusters) == 1)
