:
:	slide    executes itroff on the vax using the mslide option
:
echo "  Running files through itroff."
itroff -mslide $*
echo "  Files will be sent to the VAX at the next poll."
