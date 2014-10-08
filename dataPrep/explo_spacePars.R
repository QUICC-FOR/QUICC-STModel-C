#### Format data to insert in the STM model
#### By Steve Vissault
#### October 6th, 2014

rm(list=ls())

# Setup wd, source files, libraries -----------------------------------------------

setwd("~/Documents/GitHub/C-STMFor/")
source("dataPrep/extract_int_yrs_pastClimate.R")
source('dataPrep/format_pastClimdata_main.R')
library(ggplot2)
library(reshape2)
library(RColorBrewer)

# Find clim var space for NA value ----------------------------------------

temp <- seq(range(clim_var$avg_annual_temp)[1],range(clim_var$avg_annual_temp)[2],0.01)
pp <- seq(range(clim_var$avg_annual_pp)[1],range(clim_var$avg_annual_pp)[2],0.01)

clim_space  <- expand.grid(temp,pp)
names(clim_space)  <- c("temp","pp")

ENV1 = clim_space$temp
ENV2 = clim_space$pp

### epsilon

e0 = -5.02835512140436
e1 = -0.896555741457188
e3 = 0.110811540916957
e2 = 10.0169100493245
e4 = -5.77001585627923
e5 = 0
e6 = 0

logit_eps = e0 + e1*ENV1 + e2*ENV2 + e3*ENV1^2 + e4*ENV2^2 + e5*ENV1^3 + e6*ENV2^3
eps = exp(logit_eps)/(1+exp(logit_eps))

### alpha_b

ab0 = -1.16361552347762
ab1 = -0.123487321020654
ab3 = 0.0611134973090761
ab2 = 1.36258369833543
ab4 = 0.00617230638848189
ab5 = 0
ab6 = 0

logit_alphab = ab0 + ab1*ENV1 + ab2*ENV2 + ab3*ENV1^2 + ab4*ENV2^2 + ab5*ENV1^3 + ab6*ENV2^3
alphab = exp(logit_alphab)/(1+exp(logit_alphab))

### alpha_t

at0 = -4.25796438158399
at1 = 0.0116538718099561
at3 = 0.0537659679607981
at2 = -1.11055959620349
at4 = 4.42399157755706
at5 = 0
at6 = 0

logit_alphat = at0 + at1*ENV1 + at2*ENV2 + at3*ENV1^2 + at4*ENV2^2 + at5*ENV1^3 + at6*ENV2^3
alphat = exp(logit_alphat)/(1+exp(logit_alphat))

# Final dataset
dat  <- cbind(clim_space,eps,alphab,alphat)
ggdat  <- melt(dat, id.vars = c("pp","temp"),value.name = "prob",na.rm = TRUE)
ggdat$prob  <- cut(ggdat$prob,10)

qplot(pp, temp, data = ggdat, fill = prob, geom = "raster")  + facet_grid(~ variable) + 
  scale_fill_manual(values = rev(brewer.pal(10,"RdBu"))) + 
  xlab("Precipitation (m)") +
  ylab("Temperature (°C)")

