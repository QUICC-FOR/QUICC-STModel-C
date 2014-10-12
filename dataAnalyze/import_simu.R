### Import each simulations to a list of rasterBrick
### By Steve Vissault
### October, 10th 2014

rm(list=ls())

# Set wd and libraries------------------------------------------------------------------

setwd("~/Documents/GitHub/C-STMFor/outDir/")
library(stringr)
library(plyr)
library(raster)
library(reshape2)
library(ggplot2)


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

# Info on simulation  ------------------------------------------------------------------------
nSimu <- 60
WritingSteps  <- 10
MaxTimeSteps  <- 1000
y_Gridsize  <- 1600 
x_Gridsize  <- 500
all_steps  <- seq(WritingSteps,MaxTimeSteps,WritingSteps)
simu_paths  <- list.dirs(getwd())[str_detect(list.dirs(getwd()),"simu")]
ext  <- extent(-75,-70, 44.990798, 57.5)
proj = CRS("+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs +towgs84=0,0,0")

# Import ------------------------------------------------------------------

#### List timestep file and order them

ls_simu_order  <- simu_paths[sort(as.numeric(str_extract(simu_paths,"\\(?[0-9]+\\)?")),index.return = TRUE)$ix]
ls_files_order <- list.files(ls_simu_order[1])[sort(as.numeric(str_extract(list.files(ls_simu_order[1]),"\\(?[0-9]+\\)?")),index.return = TRUE)$ix]

#### get vector of all time steps

TimeSteps  <- as.numeric(str_extract(ls_files_order,"\\(?[0-9]+\\)?"))*WritingSteps

#### Check infos on simulation

if(nSimu != length(ls_simu_order)) stop("too many simulation folder..")
if(sum(all_steps) != sum(TimeSteps)) stop("too many TimeSteps files..")

### Create brick folder

system("mkdir 'brick_out'")

#### Run loop over all simulation folders
for(y in 1:length(ls_simu_order)){
  #### Prepare brick
  simu_path  <- ls_simu_order[y]
  
  #### Store each step in rasterbrick
  vec_file  <- paste(simu_path,ls_files_order,sep="/")
  ls_step  <- lapply(vec_file,import_step,out="spatial",WritingSteps=WritingSteps)
  brick_simu  <- brick(ls_step)
  
  brick_simu@crs  <- proj
  brick_simu@extent  <- ext
  names(brick_simu)  <- paste("simu_",TimeSteps,sep="")
  writeRaster(brick_simu, filename=paste("./brick_out/","step_",y,sep=""), format="raster",bylayer=FALSE,overwrite=TRUE)
}
  
