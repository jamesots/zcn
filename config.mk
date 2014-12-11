# This file configures ZCN's Makefiles. If you want to rebuild ZCN,
# you should edit the settings below as needed.


# Comment this out if you don't have Michael Bischoff's `cpm' CP/M
# emulator (or can't run it - quite possible if you're not building on
# an x86-based machine). Nothing vital needs this, but it does mean
# you won't be able to make `manpages.pma' or `zapdesc.bin'.
#
HAVE_CPM_EMU=yes

# Comment this out if you don't have both xearth and netpbm installed.
# These are required to build zcnclock's `cities.mrf', the source of
# the city-location graphics it uses - but you only need to remake
# this if you decide to delete it for some reason, so it's not exactly
# a big deal. :-)
#
HAVE_XEARTH_AND_NETPBM=yes
