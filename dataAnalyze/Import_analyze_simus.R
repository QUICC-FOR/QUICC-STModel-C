### Compute proportion of each step 
### By Steve Vissault
### October, 12th 2014

rm(list=ls())

# Set libraries------------------------------------------------------------------
library(raster)
library(stringr)
library(reshape2)
library(ggplot2)
library(plyr)
library(gridExtra)
library(rasterVis)

# Info on simulation  ----------------------------------------------------------------------

setwd("~/Documents/GitHub/C-STMFor/Times150-Rep100-DomPars2-CANUSA/")

nSimu <- 100
WritingSteps  <- 10
MaxTimeSteps  <-150
y_Gridsize  <- 840
x_Gridsize  <- 600
all_steps  <- seq(WritingSteps,MaxTimeSteps,WritingSteps)
simu_paths  <- list.dirs(getwd())[str_detect(list.dirs(getwd()),"simu")]
ext  <- extent(-75,-70, 44.990798, 52)
proj = CRS("+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs +towgs84=0,0,0")


# Import to rasterbrick ---------------------------------------------------
## Need to be done one times
source("../dataAnalyze/import_simu.R")

# Analyse prop ------------------------------------------------------------
setwd("./brick_out/")

# Functions ---------------------------------------------------------------
compute_prop_layer  <- function(layer,brick){
  pos=which(names(brick)==layer)
  m_simu<-as.matrix(brick[[pos]])
  #Compute prop
  prop_R<-length(which(m_simu==0))/length(m_simu)
  prop_T<-length(which(m_simu==1))/length(m_simu)
  prop_M<-length(which(m_simu==2))/length(m_simu)
  prop_B<-length(which(m_simu==3))/length(m_simu)

  out_df  <- data.frame(step=as.numeric(str_extract(layer,"[0-9]+")),prop_R=prop_R,prop_T=prop_T,prop_B=prop_B,prop_M=prop_M)
}
compute_prop_all_simus  <- function(simu_files_order){
  n_simu  <- as.factor(str_extract(simu_files_order,"[0-9]+"))
  rs_simu  <- brick(simu_files_order)
  layers  <- names(rs_simu)
  prop_layers  <- lapply(layers,compute_prop_layer,brick=rs_simu)
  prop_layers  <- do.call(rbind,prop_layers)
  prop_layers  <- data.frame(simu=rep(n_simu,nrow(prop_layers)),prop_layers)
}

# Compute prop for all simus

simu_files  <- paste(getwd(),list.files(),sep="/")[str_detect(list.files(),"grd")]
simu_files_order  <- simu_files[sort(as.numeric(str_extract(simu_files,"[0-9]+")), index.return = TRUE)$ix]

prop_all_simu  <- lapply(simu_files_order,compute_prop_all_simus)
prop_all_simu  <- do.call(rbind,prop_all_simu)
write.csv2(prop_all_simu,file="../prop_all_simus.csv")

gg_prop  <- melt(prop_all_simu,id=c("simu","step"),variable.name = "prop")
gg_prop$fac  <- paste(gg_prop$simu,gg_prop$prop)

ggplot(gg_prop,aes(x=step,y=value,group=simu,color=prop)) + geom_line(aes(group=fac)) + facet_wrap(~prop,scale="free")+ xlab("Time") +ylab("Proportion (%)") 
ggsave("../prop_time.pdf")



# Vizualisation -----------------------------------------------------------

# Load shapefile ----------------------------------------------------------
library(maptools)

# lakes  <- readShapePoly("~/Documents/Spatial_Data/CANUSA_shapes/water_area.shp")
# qc_lakes  <- crop(lakes,ext)
# writeOGR(qc_lakes, ".", "water_area_qc", driver="ESRI Shapefile")

# veg_zone  <- readShapePoly("~/Documents/Spatial_Data/CANUSA_shapes/zone_veg.shp")
# qc_veg_zone  <- crop(veg_zone,ext)
# writeOGR(qc_veg_zone, ".", "veg_zone", driver="ESRI Shapefile")

lakes  <- readShapePoly("~/Documents/Spatial_Data/CANUSA_shapes/water_area_qc.shp")
lakes@proj4string  <- proj
lakes  <- fortify(lakes)

veg_zone  <- readShapePoly("~/Documents/Spatial_Data/CANUSA_shapes/veg_zone.shp")
veg_zone@proj4string  <- proj
veg_zone  <- fortify(veg_zone)

theme_set(theme_grey(base_size = 18))
br  <- brick("simu_100.grd")
br  <- as.data.frame(br$step_150,xy=T)
br$States  <- factor(br$step_150,labels=c("Regeneration","Temperate","Mixed","Boreal"))
br  <- br[,-3]
br$scenario  <- "Actual distribution predicted"

rs_futur  <- import_step("~/Documents/GitHub/C-STMFor/Times-Rep100-DomPars-Futur-CANUSA/simu_1.csv",out="spatial")
rs_futur@crs  <- proj
rs_futur@extent  <- ext
rs_futur  <- as.data.frame(rs_futur,xy=T)
rs_futur$layer  <- factor(rs_futur$layer,labels=c("Regeneration","Temperate","Mixed","Boreal"))
names(rs_futur)[3]  <- "States" 
rs_futur$scenario  <- "Projected (100 years, +4°C)"

dat  <- rbind(br,rs_futur)
dat$scenario  <- as.factor(dat$scenario)
str(dat)

ggplot(dat) + geom_raster(aes(x,y,fill=States, order = rev(States))) +
  facet_wrap(~scenario)+
  scale_fill_brewer(palette="Spectral") +
  geom_polygon(data = subset(lakes,hole==FALSE), aes(x = long, y = lat, group = group),fill="lightskyblue",colour="dodgerblue4",size=0.1) +
  geom_polygon(data = veg_zone, aes(x = long, y = lat, group = group),fill=NA,colour="grey90",size=0.3) +
  scale_x_continuous(expand=c(0,0)) + scale_y_continuous(expand=c(0,0))+
  coord_equal() + xlab("Longitude") + ylab("Latitude")+
  theme(plot.title = element_text(lineheight=.8), panel.margin = unit(c(0.5,0,0,0),"in"),plot.margin = unit(c(0.1,0,0,0),"in"),
        strip.text.x = element_text(size = 12,face="bold" ,colour = "white"),strip.background = element_rect(colour="black", fill="black"))



