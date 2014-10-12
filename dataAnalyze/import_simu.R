### Import each simulations to a list of rasterBrick
### By Steve Vissault
### October, 10th 2014

# Functions ---------------------------------------------------------------

import_step  <- function(file,WritingSteps=WritingSteps,out=out){
  
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
  i[x=='B'] = 2
  i[x=='M'] = 3
  return(i)
}

# Import ------------------------------------------------------------------

#### List timestep file and order them
ls_files_order <- list.files(simu_paths[1])[sort(as.numeric(str_extract(list.files(simu_paths[1]),"\\(?[0-9]+\\)?")),index.return = TRUE)$ix]

#### get vector of all time steps

TimeSteps  <- as.numeric(str_extract(ls_files_order,"\\(?[0-9]+\\)?"))

#### Check infos on simulation

if(nSimu != length(simu_paths)) stop("too many simulation folder..")
if(sum(all_steps) != sum(TimeSteps)) stop("too many TimeSteps files..")

### Create brick folder

system("mkdir -p 'brick_out'")

#### Run loop over all simulation folders
for(y in 1:nSimu){
  #### Prepare brick
  #### Store each step in rasterbrick
  vec_file  <- paste(simu_paths[y],ls_files_order,sep="/")
  ls_step  <- lapply(vec_file,import_step,out="spatial",WritingSteps=WritingSteps)
  brick_simu  <- brick(ls_step)
  
  brick_simu@crs  <- proj
  brick_simu@extent  <- ext
  names(brick_simu)  <- paste("step_",TimeSteps,sep="")
  writeRaster(brick_simu, filename=paste("./brick_out/","step_",y,sep=""), format="raster",bylayer=FALSE,overwrite=TRUE)
}
  
