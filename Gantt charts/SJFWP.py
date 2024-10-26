import Gantt

events = [

    #For the first 6 seconds, p1 is the only job, so do that. (remaining time of p1: 7s)
    {'Event': 'P1'},

    # After that, P2 arrives and is the shortest task (4 seconds) so it is completed
    {'Event': 'P2'},

    #For the remaining 5 seconds before the arrival of p3, do event P1 (2s left)
    {'Event': 'P1'},
    #P3 arrives but p1 has 2 seconds left so we wait till 17 seconds in
    #p3 goes for 4 seconds (7 seconds remaining at 21s in
    {'Event': 'P3'},
    #P4 arrives but we stick with P3 till 28s
    #At 28s p4 starts forever till 37
    {'Event': 'P4'},
    {'Event': 'P5'}, # Then 

]

divisions = [
        0,
        6,
        10,
        17,
        28,
        37,
        50
]

Gantt.drawGantt(events, divisions,"Shortest Job First With Preemption")