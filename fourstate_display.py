import sys
import time
import tkDisplay

def interpretVal(value):
    if value == 'D':
        result = 0
    elif value == 'M':
        result = 1
    elif value == 'C':
        result = 2
    else:
        result = 3
    return result


fname = sys.argv[1]
waitTime = 0.1  # number of seconds to wait between successive refreshes

# read file and parse into data structure
gridData = dict()
nYears = None
xMax = yMax = 0
xMin = yMin = 10000000000

with open(fname) as f:
    lines = f.readlines()
    for line in lines:
        line = line.rstrip()
        line = line.replace('"', '')
        line = line.split(',')
        
        coords = (int(line[0]), int(line[1]))
        if coords[0] < xMin:
            xMin = coords[0]
        elif coords[0] > xMax:
            xMax = coords[0]
        if coords[1] < yMin:
            yMin = coords[1]
        elif coords[1] > yMax:
            yMax = coords[1]
        gridData[coords] = line[2:]
        if(nYears) is None:
            nYears = len(gridData[coords])

display = tkDisplay.tkDisplay((xMax - xMin + 1, yMax - yMin + 1), displayCellSize=10)

for year in range(nYears):
    stTime = time.clock()
    grid = []
    for coords in gridData.keys():
        grid.append((coords[0], coords[1], interpretVal(gridData[coords][year])))
    display.update(grid)
    raw_input("Press Enter to continue")
#    if(year == 0):
#        raw_input("Press enter to begin")
#    elapsed = time.clock() - stTime
#    if elapsed < waitTime:
#        time.sleep(waitTime - elapsed)



raw_input("Press enter to exit")

