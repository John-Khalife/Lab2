import Gantt

events = [
    # p1 runs for the full 6 seconds. During this time, it goes from h-p to m-p. There are 10 seconds remaining.
    {'Event': 'P1'},
    {'Event': 'IO'},
    {'Event': 'P1'},
    {'Event': 'IO'},
    {'Event': 'P1'},
    {'Event': 'IO'},


    #p2 Arrives at 6s. It executes for 2s, and p1 for 2s, then p2 for 2more s., and 1 for p1 this goes to 13s
    #p1 is at 7s, p2 is finished.
    {'Event': 'P2'},
    {'Event': 'P1'},
    {'Event': 'P2'},
    {'Event': 'P1'},
    {'Event': 'P2'},
    {'Event': 'P1'},
    {'Event': 'P2'},

    {'Event': 'P1'},
    {'Event': 'IO'}, #p1 at 6s

    #p3 arrives. executes for 3s, alternating with p1 till 21s
    # 8 left on p3, 3 left on p1
    {'Event': 'P3'},
    {'Event': 'P1'},
    {'Event': 'P3'},
    {'Event': 'P1'},
    {'Event': 'P3'},
    {'Event': 'P1'},

    #p4 arrives. executes for 1s, then p3, then p4. p4 at 7s, p3 at 7ss
    {'Event': 'P4'},
    {'Event': 'P3'},
    {'Event': 'P4'},

    #p5 for 1s, p3 for 1s, p5 for 1s then once more for p3 6s, p4 at 11s
    {'Event': 'P5'},
    {'Event': 'P3'},
    {'Event': 'P5'},

    #p4 at 4s, p5  at 7s
    {'Event': 'P4'},
    {'Event': 'P5'},
    {'Event': 'P4'},
    {'Event': 'P5'},
    {'Event': 'P4'},
    {'Event': 'P5'},


    #p4 finishes, p3 finishes
    {'Event': 'P1'},
    {'Event': 'P3'},
    {'Event': 'P1'},
    {'Event': 'P3'},
    {'Event': 'P1'},
    {'Event': 'P3'},
    {'Event': 'P4'},
    {'Event': 'P3'},
    {'Event': 'P4'},
    {'Event': 'P3'},
    {'Event': 'P4'},
    {'Event': 'P3'},
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
        61
]

Gantt.drawGantt(events, divisions,"Multilevel Queue with IO")