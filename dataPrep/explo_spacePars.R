#### Format data to insert in the STM model
#### By Steve Vissault
#### October 6th, 2014

rm(list=ls())

################### DOM PARAMETERS

# Setup wd, source files, libraries -----------------------------------------------

setwd("~/Documents/GitHub/C-STMFor/")
source("dataPrep/extract_int_yrs_pastClimate.R")
source('dataPrep/format_pastClimdata_main.R')
library(ggplot2)
library(reshape2)
library(RColorBrewer)
library(gridExtra)

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
dom_dat  <- cbind(clim_space,eps,alphab,alphat)
ggdat  <- melt(dom_dat, id.vars = c("pp","temp"),value.name = "prob",na.rm = TRUE)
ggdat$prob  <- cut(ggdat$prob,10)

dom_plot  <- qplot(pp, temp, data = ggdat, fill = prob, geom = "raster")  + facet_grid(~ variable) + 
  scale_fill_manual(values = rev(brewer.pal(10,"RdBu")),guide = guide_legend(reverse=TRUE)) + 
  xlab("Precipitation (m)") +
  ylab("Temperature (°C)") +
  ggtitle("Dom parameters") 

################### ISA PARAMETERS - M3

ab0 = -5.83310120634012
ab1 = 0.302765279275565
ab2 = -0.294089983277681
ab3 = 10.8333695080604
ab4 = -5.62990151821534
ab5 = 0.0640096686470137
ab6 = 1.02231089391224
at0 = -6.08703694314924
at1 = 1.12125814098391
at2 = -0.513342458709639
at3 = 4.97495833072608
at4 = 3.70643302670873
at5 = 0.0713962200708099
at6 = -3.11904824074024
bb0 = -0.830745822065995
bb1 = 1.4957707612483
bb2 = -0.437338022347829
bb3 = -0.612605811838059
bb4 = -0.269617897223035
bb5 = 0.0276871319205698
bb6 = -0.416330816107804
bt0 = -4.13729695769195
bt1 = -1.1060570319351
bt2 = 0.467080781441068
bt3 = 3.97746492387707
bt4 = 2.33605025415756
bt5 = -0.0524500050226244
bt6 = -2.78283252405642
tt0 = -4.86852023621565
tt1 = 0.715117863288292
tt2 = -0.0345509917337016
tt3 = 0.265782459855644
tt4 = -3.33051339303483
tt5 = -0.00471214386561885
tt6 = 2.3959411808424
tb0 = -14.4074362708654
tb1 = 7.41468735588291
tb2 = -1.11964123224213
tb3 = -4.05893490750906
tb4 = 8.55489201150745
tb5 = -0.0598259975911364
tb6 = -4.78361972044024
e0 = -0.862451460856885
e1 = -0.656876199857483
e2 = -0.124536965936071
e3 = 0.305128796131605
e4 = 1.19351458229033
e5 = 0.035549677324977
e6 = -1.33427070640359


logit_alphab     = ab0 + ab1*ENV1 + ab2*ENV1^2 + ab3*ENV2 + ab4*ENV2^2 + ab5*ENV1^3 + ab6*ENV2^3
logit_alphat     = at0 + at1*ENV1 + at2*ENV1^2 + at3*ENV2 + at4*ENV2^2 + at5*ENV1^3 + at6*ENV2^3
logit_betab     = bb0 + bb1*ENV1 + bb2*ENV1^2 + bb3*ENV2 + bb4*ENV2^2 + bb5*ENV1^3 + bb6*ENV2^3
logit_betat     = bt0 + bt1*ENV1 + bt2*ENV1^2 + bt3*ENV2 + bt4*ENV2^2 + bt5*ENV1^3 + bt6*ENV2^3
logit_thetab    = tb0 + tb1*ENV1 + tb2*ENV1^2 + tb3*ENV2 + tb4*ENV2^2 + tb5*ENV1^3 + tb6*ENV2^3
logit_thetat    = tt0 + tt1*ENV1 + tt2*ENV1^2 + tt3*ENV2 + tt4*ENV2^2 + tt5*ENV1^3 + tt6*ENV2^3
logit_eps     = e0  + e1*ENV1  + e2*ENV1^2 + e3*ENV2 + e4*ENV2^2 + e5*ENV1^3 + e6*ENV2^3


### epsilon
eps = exp(logit_eps)/(1+exp(logit_eps))

### alpha_b
alphab = exp(logit_alphab)/(1+exp(logit_alphab))

### alpha_t
alphat = exp(logit_alphat)/(1+exp(logit_alphat))

# Final dataset
isa_dat  <- cbind(clim_space,eps,alphab,alphat)
ggdat  <- melt(isa_dat, id.vars = c("pp","temp"),value.name = "prob",na.rm = TRUE)
ggdat$prob  <- cut(ggdat$prob,10)

isa_plot_m3  <- qplot(pp, temp, data = ggdat, fill = prob, geom = "raster")  + facet_grid(~ variable) + 
  scale_fill_manual(values = rev(brewer.pal(10,"RdBu")),guide = guide_legend(reverse=TRUE))+
  xlab("Precipitation (m)") +
  ylab("Temperature (°C)")+
  ggtitle("Isa parameters")


################### ISA PARAMETERS - M3

ab0 = -3.06273148094673 
ab1 = 0.186575856145222
ab2 = 0.0903912192859453
ab3 = 6.85966270860905
ab4 = -8.14592839370102
ab5 = -0.0169839806038239
ab6 = 4.10319460225323
at0 = -11.8033456001239
at1 = 1.90112374794202
at2 = -0.783402552677859
at3 = 16.8106170490648
at4 = -8.71750378478698
at5 = 0.109198439825065
at6 = 2.00059804014306
bb0 = -3.84621664090099
bb1 = 1.39492510407842
bb2 = -0.411126102830225
bb3 = 6.11028742414826
bb4 = -1.4121071143611
bb5 = 0.0317996218979764
bb6 = -2.63883802545558
bt0 = -4.28816752480749
bt1 = -0.261267407368761
bt2 = 0.0264375341083878
bt3 = 2.67381691612993
bt4 = 2.92669812532092
bt5 = 0.00105202025795807
bt6 = -2.27884616411107
tt0 = -8.83255983394709
tt1 = 0.715945346731162
tt2 = 0.0547809437006877
tt3 = 5.71377347108633
tt4 = -6.88900566391751
tt5 = -0.0199439060390264
tt6 = 3.89108840559433
tb0 = 6.28716175814805
tb1 = 3.0536973203079
tb2 = 1.02053277360226
tb3 = -24.3354683120603
tb4 = -4.06765604943657
tb5 = -0.315577010278761
tb6 = 8.09883199699756
e0 = 1.40622556633735 
e1 = -0.65394322310289 
e2 = -0.0571679771637695 
e3 = -1.26116837213423 
e4 = -2.40588097652592 
e5 = 0.0220757297953012 
e6 = 1.30127537674402 


logit_alphab     = ab0 + ab1*ENV1 + ab2*ENV1^2 + ab3*ENV2 + ab4*ENV2^2 + ab5*ENV1^3 + ab6*ENV2^3
logit_alphat     = at0 + at1*ENV1 + at2*ENV1^2 + at3*ENV2 + at4*ENV2^2 + at5*ENV1^3 + at6*ENV2^3
logit_betab     = bb0 + bb1*ENV1 + bb2*ENV1^2 + bb3*ENV2 + bb4*ENV2^2 + bb5*ENV1^3 + bb6*ENV2^3
logit_betat     = bt0 + bt1*ENV1 + bt2*ENV1^2 + bt3*ENV2 + bt4*ENV2^2 + bt5*ENV1^3 + bt6*ENV2^3
logit_thetab    = tb0 + tb1*ENV1 + tb2*ENV1^2 + tb3*ENV2 + tb4*ENV2^2 + tb5*ENV1^3 + tb6*ENV2^3
logit_thetat    = tt0 + tt1*ENV1 + tt2*ENV1^2 + tt3*ENV2 + tt4*ENV2^2 + tt5*ENV1^3 + tt6*ENV2^3
logit_eps     = e0  + e1*ENV1  + e2*ENV1^2 + e3*ENV2 + e4*ENV2^2 + e5*ENV1^3 + e6*ENV2^3


### epsilon
eps = exp(logit_eps)/(1+exp(logit_eps))

### alpha_b
alphab = exp(logit_alphab)/(1+exp(logit_alphab))

### alpha_t
alphat = exp(logit_alphat)/(1+exp(logit_alphat))

# Final dataset
isa_dat  <- cbind(clim_space,eps,alphab,alphat)
ggdat  <- melt(isa_dat, id.vars = c("pp","temp"),value.name = "prob",na.rm = TRUE)
ggdat$prob  <- cut(ggdat$prob,10)

isa_plot_m3_veg  <- qplot(pp, temp, data = ggdat, fill = prob, geom = "raster")  + facet_grid(~ variable) + 
  scale_fill_manual(values = rev(brewer.pal(10,"RdBu")),guide = guide_legend(reverse=TRUE))+
  xlab("Precipitation (m)") +
  ylab("Temperature (°C)")+
  ggtitle("Isa parameters")


grid.arrange(dom_plot,isa_plot,nrow=2)
