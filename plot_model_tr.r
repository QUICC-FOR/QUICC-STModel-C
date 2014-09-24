library(reshape2)
#setwd('/Users/mtalluto/Documents/git_projects/C-STMFor')

inFile = commandArgs(TRUE)[1]
outFile = commandArgs(TRUE)[2]
# inFile = "stTest.csv"
# outFile = ""
# 
state_to_num = function(x) {
	i = numeric(length(x))
	i[x=='R'] = 0
	i[x=='T'] = 1
	i[x=='B'] = 2
	i[x=='M'] = 3
	return(i)
}

caData = read.table(inFile, header=F, sep=',')
colnames(caData) = c('x','y','state')
caData$state = state_to_num(caData$state)
zData = acast(caData, rev(y)~x, value.var = 'state')

colors = c(
	"#ffff99", #R
	"#7fc97f", #T
	"#beaed4", #B
	"#fdc086") #M

height = 7
# quartz(height = height, width = height)
pdf(height=height, width=height, file=outFile)
par(mar=c(3.5,3.5,2,1), bty='n', xpd=T)
precipVals = seq(1000, 1300, 100)
xTickLocs = seq(min(caData$x), max(caData$x) + 1, length.out=length(precipVals))

tempVals = rev(seq(6, 1))
yTickLocs = seq(min(caData$y), max(caData$y) + 1, length.out=length(tempVals))

image((min(caData$y):max(caData$y)), (min(caData$x):max(caData$x)), zData, xlab='', ylab='', xaxt='n', yaxt='n', xlim=range(yTickLocs), ylim=range(xTickLocs), col=colors)
axis(side=2, at=xTickLocs, labels=as.character(precipVals))
mtext(side=2, line=2, text="Precipitation (mm)")
axis(side=1, at=yTickLocs, labels=as.character(tempVals))
mtext(side=1, line=2, text="Temperature")
legend(min(caData$y), 1.05*max(caData$x), legend=c('R', 'T', 'B', 'M'), fill=colors, horiz=TRUE, bty='n')
dev.off()