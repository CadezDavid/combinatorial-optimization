TOPDIR=$(realpath $(dirname $0))
BLOSSOM_DIR=$TOPDIR/blossom5-v2.05.src

if [ -e $BLOSSOM_DIR ] ; then
   echo "Deleting old blossom5 install..."
   rm -r $BLOSSOM_DIR
fi

echo "Downloading blossom5..."
cd $TOPDIR
wget https://pub.ista.ac.at/~vnk/software/blossom5-v2.05.src.tar.gz
tar -xf blossom5-v2.05.src.tar.gz
rm blossom5-v2.05.src.tar.gz

echo "Adapting Makefile for static library builds..."
cd $BLOSSOM_DIR
sed -i "s/ blossom5/blossom5 blossom5.a/" Makefile
printf '\nblossom5.a: ${OBJS}\n	ar crv blossom5.a ${OBJS}' >> Makefile

echo "Building blossom5-v2.05..."
make
EXITSTATUS=$?
echo

