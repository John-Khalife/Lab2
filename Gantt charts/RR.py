import Gantt

events = [
    {'Event': 'P1'},
    {'Event': 'P2'},
    {'Event': 'P1'},
    {'Event': 'P3'},
    {'Event': 'P1'},
    {'Event': 'P3'},
    {'Event': 'P4'},
    {'Event': 'P5'},
    {'Event': 'P3'},
    {'Event': 'P4'},
    {'Event': 'P5'},
    {'Event': 'P4'},
    {'Event': 'P5'}
]

divisions = [
        0,
        8,
        12,
        16,
        20,
        21,
        25,
        29,
        33,
        36,
        40,
        44,
        45,
        50
        
]

Gantt.drawGantt(events, divisions,"Round Robin")