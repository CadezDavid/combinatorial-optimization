# Example script for compiling

TOPDIR=$(realpath $(dirname $0))
BLOSSOM_DIR=$TOPDIR/blossom5-v2.05.src

if [ ! -d $BLOSSOM_DIR ] ; then
   $TOPDIR/setup_blossomv.sh
fi

echo "make executable ..."
cd $TOPDIR/src
make $*
EXITSTATUS=$?
echo ""

# Create bin dir with link to executable
if [[ $EXITSTATUS == 0 ]]; then
  echo "Creating $TOPDIR/bin/minimum-weight-perfect-matching ..."
  mkdir -p $TOPDIR/bin
  ln -sf $TOPDIR/src/build/minimum-weight-perfect-matching $TOPDIR/bin/
  echo "done."
else
  echo "Compilation failed"
fi

exit $EXITSTATUS

