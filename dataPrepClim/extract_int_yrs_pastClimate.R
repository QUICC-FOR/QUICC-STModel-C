## Script to prepare climate raster
## By Steve Vissault
## October 5th, 2014

# Climatic variables 

#"bio300_01"  "Bio01"	"Annual mean temperature"
#"bio300_06"	"Bio06"	"Minimum temperature of the coldest month"
#"bio300_10"	"Bio10"	"Mean temperature of the warmest quarter"
#"bio300_11"	"Bio11"	"Mean temperature of the coldest quarter"
#"bio300_12"	"Bio12"	"Total annual precipitation"
#"bio300_15"	"Bio15"	"Precipitation seasonality"


#######################################################################
#### Get_bioclim_var
#######################################################################
# path_var: Corresponds to the path folder contening the bioclim variable
# yearMAX & yearMIN: specificy the time interval in years
# RETURN: rasterstack of the bioclim variable and the time interval specified

get_bioclim_var  <- function(path_var, yearMin, yearMax){
  
  library(raster)
  library(rgdal)
  library(stringr)
  
  # get files ids corresponding to the years
  id_climdata_yrs  <- as.character(c(seq(30,99,1),str_c('0',seq(0,9,1)),10))
  real_yrs  <- as.numeric(seq(1930,2010,1))
  
  id_files_yrs  <- str_c('_',id_climdata_yrs[which(real_yrs == yearMin):which(real_yrs == yearMax)],'.asc')
  
  # List of files
  rs_files  <- list.files(path_var)
  rm_files  <- which(str_detect(rs_files,c('.aux')))
  
  if(!length(rm_files)==0) rs_files  <- rs_files[-rm_files]
  
  # Retrieve all files in time interval
  out_match  <- as.numeric(length(id_files_yrs))
  
  for(i in 1:length(id_files_yrs)){
    out_match[i]  <- which(str_detect(rs_files,id_files_yrs[i]))
  }
  
  files_interval  <- rs_files[out_match]
  
  # Start importation
  ###########################
  
  # Extent Québec + 1° 
  #ext_crop <- extent(-80.7600089999999,-56.1054839999999, 45.990798, 63.5852190000001)
  ext_crop  <- extent(-75,-70, 44.990798, 52)
  
  ## Create rasterstack
  for(i in 1:length(files_interval)){
    if(i == 1) {
      r  <- raster(str_c(path_var,files_interval[i]),crs=CRS("+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs +towgs84=0,0,0")) 
      r  <- crop(r, ext_crop)
      rs  <- stack(r)
    } else {
      r  <- raster(str_c(path_var,files_interval[i]), crs= CRS("+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs +towgs84=0,0,0"))
      r  <- crop(r, ext_crop)
      rs  <-  stack(rs,r)
    }
  }
  
  names(rs)  <- seq(yearMin,yearMax,1)
  return(rs)
  
}

  


