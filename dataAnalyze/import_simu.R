### Import each simulations to a list of rasterBrick
### By Steve Vissault
### October, 10th 2014

# Functions ---------------------------------------------------------------

source("~/Documents/GitHub/C-STMFor/dataAnalyze/import_step.R")

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
  writeRaster(brick_simu, filename=paste("./brick_out/","simu_",y,sep=""), format="raster",bylayer=FALSE,overwrite=TRUE)
}
  
