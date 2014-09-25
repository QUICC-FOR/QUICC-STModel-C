library(reshape2)

if(interactive()) {
	setwd('/Users/mtalluto/Documents/git_projects/C-STMFor')
	inDir = 'modelTest'
} else {
	inDir = commandArgs(TRUE)[1]
	outFile = commandArgs(TRUE)[2]
}

allStates = c('R', 'T', 'B', 'M')
bigMajority = 0.75
majority = 0.5
plurality = 0.4

ca.add = function(file, addDF = NULL) {
	dat = read.table(file, header=F, sep=',')
	colnames(dat) = c('x','y','state')
	if(is.null(addDF))
		addDF = data.frame(x=dat$x, y = dat$y, R=rep(0,nrow(dat)), T=rep(0,nrow(dat)), B=rep(0,nrow(dat)), M=rep(0,nrow(dat)))

	result = merge(dat, addDF)
	for(st in allStates)
		result[result$state==st,st] = result[result$state==st,st] + 1

	result[,-which(names(result) == 'state')]
}

state_to_num = function(x) {
	i = numeric(length(x))
	i[x=='0'] = 0
	
	i[x=='R-'] = 1
	i[x=='R'] = 2
	i[x=='R+'] = 3

	i[x=='T-'] = 4
	i[x=='T'] = 5
	i[x=='T+'] = 6

	i[x=='B-'] = 7
	i[x=='B'] = 8
	i[x=='B+'] = 9

	i[x=='M-'] = 10
	i[x=='M'] = 11
	i[x=='M+'] = 12
	return(i)
}

num_to_state = function(x) {
	i = numeric(length(x))
	i[x==0] = '0'
	
	i[x==1] = 'R-'
	i[x==2] = 'R'
	i[x==3] = 'R+'

	i[x==4] = 'T-'
	i[x==5] = 'T'
	i[x==6] = 'T+'

	i[x==7] = 'B-'
	i[x==8] = 'B'
	i[x==9] = 'B+'

	i[x==10] = 'M-'
	i[x==11] = 'M'
	i[x==12] = 'M+'
	return(i)
}

num_to_state_lab = function(x) {
	i = vector('character', length(x))
	i[x == 0] = "tie"
	i[x >= 1 & x <= 3] = "R"
	i[x >= 4 & x <= 6] = "T"
	i[x >= 7 & x <= 9] = "B"
	i[x >= 10 & x <= 12] = "M"
	
	return(i)
}

state_lab_to_col = function(x) {
	i = vector('character', length(x))
	i[x=='tie'] = "#ffffff"
	i[x=='R'] = "#FCFC47"
	i[x=='T'] = "#7fc97f"
	i[x=='B'] = "#AF82ED"
	i[x=='M'] = "#fdc086"
	
	return(i)
}




allFiles = list.files(path=inDir, pattern="*.csv", full.names=T)
numTests = length(allFiles)

caDataSum = NULL
for(currentFile in allFiles)
	caDataSum = ca.add(currentFile, caDataSum)
	
caDataProp = caDataSum
caDataProp[,3:6] = caDataProp[,3:6]/numTests
caData = data.frame(x=caDataProp$x, y=caDataProp$y, state=rep("0", nrow(caDataProp)), stringsAsFactors=FALSE)

# decide which state "wins"
for(st in allStates) {
	# big majority & majority
	caData$state[caDataProp[,st] > bigMajority] = paste(st, '+', sep='')
	caData$state[caDataProp[,st] <= bigMajority & caDataProp[,st] > majority] = st
	
	# decide plurality (greater than every state, but <= simple majority
	candidates = (caData$state == '0' & caDataProp[,st] > plurality)
	maxVals = (caDataProp[,st] == apply(caDataProp[,3:6], 1, max))
	caData$state[candidates & maxVals] = paste(st, '-', sep='')
}



caData$state = state_to_num(caData$state)
zData = acast(caData, x~y, value.var = 'state')

state_to_col = function(x) {
	i = vector('character', length(x))
	i[x=='0'] = "#ffffff"
	
	i[x=='R-'] = "#ffff99"
	i[x=='R'] = "#FCFC47"
	i[x=='R+'] = "#E0E000"

	i[x=='T-'] = "#BEE8BE"
	i[x=='T'] = "#7fc97f"
	i[x=='T+'] = "#539953"

	i[x=='B-'] = "#beaed4"
	i[x=='B'] = "#AF82ED"
	i[x=='B+'] = "#7F3BDB"

	i[x=='M-'] = "#FADEC3"
	i[x=='M'] = "#fdc086"
	i[x=='M+'] = "#E68A32"
	
	return(i)
}


# subset colors to only include states that occur in the final plot
colors=state_to_col(num_to_state(unique(as.vector(zData))))
colors=c(
	"#ffffff",
	
	"#ffff99",
	"#FCFC47",
	"#E0E000",

	"#BEE8BE",
	"#7fc97f",
	"#539953",

	"#beaed4",
	"#AF82ED",
	"#7F3BDB",
	
	"#FADEC3",
	"#fdc086",
	"#E68A32")
asp = max(caData$x)/max(caData$y)
height = 10
width = height * asp
if(interactive()) {
	quartz(height = height, width = width)
} else {
	pdf(height=height, width=width, file=outFile)
}
par(mar=c(3.5,3.5,2,1), bty='n', xpd=T)
precipVals = seq(1000, 1300, 100)
xTickLocs = seq(min(caData$x), max(caData$x) + 1, length.out=length(precipVals))

tempVals = seq(6, 1)
yTickLocs = seq(min(caData$y), max(caData$y) + 1, length.out=length(tempVals))

image((min(caData$x):max(caData$x)), (min(caData$y):max(caData$y)), zData, xlab='', ylab='', xaxt='n', yaxt='n', xlim=range(xTickLocs), ylim=range(yTickLocs), col=colors, zlim=c(-1,13))
#image((min(caData$x):max(caData$x)), (min(caData$y):max(caData$y)), zData, xlab='', ylab='', xlim=range(xTickLocs), ylim=range(yTickLocs), col=colors)
axis(side=1, at=xTickLocs, labels=as.character(precipVals))
mtext(side=1, line=2, text="Precipitation (mm)")
axis(side=2, at=yTickLocs, labels=as.character(tempVals))
mtext(side=2, line=2, text="Temperature")
labs = unique(num_to_state_lab((0:12)[which(0:12 %in% unique(as.vector(zData)))]))
legend(min(caData$x), 1.05*max(caData$y), legend=labs, fill=state_lab_to_col(labs), horiz=TRUE, bty='n')
if(!interactive()) dev.off()