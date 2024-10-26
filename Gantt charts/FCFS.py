import Gantt


events = [
    {'Event': 'P1'},
    {'Event': 'P2'},
    {'Event': 'P3'},
    {'Event': 'P4'},
    {'Event': 'P5'}
]

divisions = [
        0,
        13,
        17,
        28,
        37,
        50
]

Gantt.drawGantt(events,divisions,"First Come First Serve")

