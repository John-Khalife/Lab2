import Gantt

events = [
    # p1 runs for the full 6 seconds. During this time, it goes from h-p to m-p to 1s l-p. There are 7 seconds remaining.
    {'Event': 'P1'},
    #p2 Arrives at 6s. It executes in h-p, then finishes in m-p (4 seconds) at 10s
    {'Event': 'P2'},
    # p1 executes for the next 5s - 2s remaining
    {'Event': 'P1'},
    # p3 is triggered. runs for 5s until 20s. with 6s left.
    {'Event': 'P3'},
    #p1 runs for one second., 1s remaining
    {'Event': 'P1'},
    #p4 runs for 3 seconds - down to medium queue
    {'Event': 'P4'},
    #p5 runs for 2 seconds
    {'Event': 'P5'},
    #p4 runs for 2 seconds
    {'Event': 'P4'},
    #p5 runs for 3 seconds
    {'Event': 'P5'},
    #p1 for 1 second finishes
    {'Event': 'P1'},
    #p3 for 6 second finishes
    {'Event': 'P3'},
    #p4 runs for the rest
    {'Event': 'P4'},
    #then p5 for the rest
    {'Event': 'P5'},



]

divisions = [
        0,
        6,
        10,
        15,
        20,
        21,
        24,
        26,
        28,
        31,
        32,
        38,
        42,
        50


        
]

Gantt.drawGantt(events, divisions,"Multilevel Queue")