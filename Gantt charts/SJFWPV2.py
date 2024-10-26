import Gantt

events = [
    #First 6 seconds b4 p2, 10 seconds left on p1
    {'Event': 'P1'},
    {'Event': 'IO'},
    {'Event': 'P1'},
    {'Event': 'IO'},
    {'Event': 'P1'},
    {'Event': 'IO'},
    
    #p2 arrives, iterate between p1 and p2 until 13s
    {'Event': 'P2'},
    {'Event': 'P1'},
    {'Event': 'P2'},
    {'Event': 'P1'},
    {'Event': 'P2'},
    {'Event': 'P1'},
    {'Event': 'P2'},

    #Only p1 for the next 2 second till 15 - 6 seconds remaining on p1
    {'Event': 'P1'},
    {'Event': 'IO'},

    # p3 and p1 toggle until 21 - 8 left on p3, 3 left on p1
    {'Event': 'P1'},
    {'Event': 'P3'},
    {'Event': 'P1'},
    {'Event': 'P3'},
    {'Event': 'P1'},
    {'Event': 'P3'},

    #p4 arrives, finish p1 at 27, 5s left on p3
    {'Event': 'P1'},
    {'Event': 'P3'},
    {'Event': 'P1'},
    {'Event': 'P3'},
    {'Event': 'P1'},
    {'Event': 'P3'},

    #p3 finished, p4 at 4s left
    {'Event': 'P4'},
    {'Event': 'P3'},
    {'Event': 'P4'},
    {'Event': 'P3'},
    {'Event': 'P4'},
    {'Event': 'P3'},
    {'Event': 'P4'},
    {'Event': 'P3'},
    {'Event': 'P4'},
    {'Event': 'P3'},

    {'Event': 'P4'},
    {'Event': 'P5'},
    {'Event': 'P4'},
    {'Event': 'P5'},
    {'Event': 'P4'},
    {'Event': 'P5'},
    {'Event': 'P4'},
    {'Event': 'P5'},

    {'Event': 'IO'},
    {'Event': 'P5'},
    {'Event': 'IO'},
    {'Event': 'P5'},
    {'Event': 'IO'},
    {'Event': 'P5'},
    {'Event': 'IO'},
    {'Event': 'P5'},
    {'Event': 'IO'},
    {'Event': 'P5'},
    {'Event': 'IO'},
    {'Event': 'P5'},
    {'Event': 'IO'},
    {'Event': 'P5'},
    {'Event': 'IO'},
    {'Event': 'P5'},
    {'Event': 'IO'},
    {'Event': 'P5'},







]

divisions = [
        0,
        1,
        2,
        3,
        4,
        5,
        6,


        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15,
        16,
        17,
        18,
        19,
        20,
        21,
        22,
        23,
        24,
        25,
        26,
        27,
        28,
        29,
        30,
        31,
        32,
        33,
        34,
        35,
        36,
        37,
        38,
        39,
        40,
        41,
        42,
        43,
        44,
        45,
        46,
        47,
        48,
        49,
        50,
        51,
        52,
        53,
        54,
        55,
        56,
        57,
        58,
        59,
        60,
        61,
        62,
        63,
]

Gantt.drawGantt(events,divisions,"Preemptive Shortest Job First with IO")

