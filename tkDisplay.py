import Tkinter

class tkDisplay(object):
    def __init__(self, dimensions = 10,  startingGrid = None, displayCellSize = 5):
        # if we only get passed one value, square array
        if( (type(dimensions) is int) or (len(dimensions) == 1) ):
            self.dimensions = (dimensions, dimensions)
        else:
            self.dimensions = dimensions

        self.displayCellSize = displayCellSize
        
        ### initialize the display
        self.displayWindow = gridTk(int(self.displayCellSize), self.dimensions)
        if(startingGrid is None):
            startingGrid = []
            for(xx, yy) in zip(range(self.dimensions[0]), range(self.dimensions[1])):
                startingGrid.append((xx, yy, 0))
        self.update(startingGrid)

#    def update(self, grid, colRamp = ['#004400', '#339933', '#77cc77', 'white']):
    def update(self, grid, colRamp = ['#7fc97f', '#beaed4', '#fdc086', 'white', 'black']):
        """ update grid display with the current grid contents """
        # update the cell colors
        for cell in grid:
            # this assumes the coordinates in cell are indexed to 0, and 0 is the southernmost point
            # add one, because the display is indexed to 1, not 0
            # flip y coordinates to make them correspond with geographical coordinates
            yl = self.dimensions[1] - cell[1]
            xl = cell[0] + 1
            color = colRamp[int(cell[2])]
            self.displayWindow.update((xl, yl), color)

        # refresh the window
        self.displayWindow.tkupdate()


class gridTk(object):
    def __init__(self, cellSize, gridDim):

        self.done = 0   # exit flag set to False

        self.cells = []
        self.row = []
        self.cellSize = cellSize
        self.gridSize = gridDim
        pixels = (self.gridSize[0] * self.cellSize, self.gridSize[1] * self.cellSize)
        self.a = {}                # what the hell is a????

        # set up Tk

        self.tk = Tkinter.Tk()
        self.tk.wm_geometry(str(pixels[0]) + 'x' + str(pixels[1]) + '+20+40')
        self.tk.bind("<Return>", self.finish)
        self.canvas = Tkinter.Canvas(self.tk, width=pixels[0], height=pixels[1])
        self.canvas.pack()

        y=0

        for i in range(0, self.gridSize[0] * self.cellSize, self.cellSize):
            x=0
            y += 1
            for j in range(0, self.gridSize[1] * self.cellSize, self.cellSize):
                x += 1
                self.a[(y,x)] = \
                    self.canvas.create_rectangle(i, j, i + self.cellSize, \
                                                 j + self.cellSize)#, fill='gray')

        self.tk.update()

    def finish(self, e):
        self.done = 1

    def update(self, cellRef, color):
        # put your update code here
        ## cellRef is 0 indexed, so we add 1 for the indexing of the display
        self.canvas.itemconfigure(self.a[cellRef], fill=color, outline=color)
        
    def tkupdate(self):
        self.tk.update()

