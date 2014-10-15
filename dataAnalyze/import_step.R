### Import each step to a raster
### By Steve Vissault
### October, 10th 2014

# Functions ---------------------------------------------------------------

import_step  <- function(file,out=out){
  
  TimeStepFile  <- read.csv(file,header = F)
  names(TimeStepFile)  <- c("x","y","state")
  
  step  <- as.numeric(str_extract(file,"\\(?[0-9]+\\)?"))*WritingSteps
  
  TimeStepFile$x  <- TimeStepFile$x+1 
  TimeStepFile$y  <- TimeStepFile$y+1
  TimeStepFile$state  <- state_to_num(TimeStepFile$state)
  
  if(out == "df"){
    return(data.frame(TimeStepFile[,1:2],step=rep(step,nrow(TimeStepFile),TimeStepFile[,3])))
  }
  
  if (out == "spatial"){
    # Transform df as raster
    TimeStepFile  <- matrix(acast(TimeStepFile,rev(y)~x,value.var = "state"),
                            nrow=max(TimeStepFile$y),ncol=max(TimeStepFile$x))
    rs_TimeStepFile  <- raster(TimeStepFile)
    return(rs_TimeStepFile)
  }
}
state_to_num  <- function(x) {
  i = numeric(length(x))
  i[x=='R'] = 0
  i[x=='T'] = 1
  i[x=='M'] = 2
  i[x=='B'] = 3
  return(i)
}
