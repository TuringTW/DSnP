rm self.result
rm ref.result
rm compare
echo "===running(Ric)==="
../ref/taskMgr -f do2 2> ref.result 1>&2
echo "===running(Kevin)==="
../taskMgr -f do2 2> self.result 1>&2
echo "===diff======"
diff self.result ref.result | tee compare
