import Gantt

data = {
    'Event': [
        'P1',
        'P2',
        'P3',
        'P4',
        'P5'
    ],
    'Start': [
        0,
        13,
        17,
        28,
        37
    ],
    'End': [
        13,
        17,
        28,
        37,
        50
    ],
    'Color': [
        'blue',   # Color for P1
        'orange',  # Color for P2
        'green',   # Color for P3
        'red',     # Color for P4
        'purple'   # Color for P5
    ]
}

Gantt.drawGantt(data)

