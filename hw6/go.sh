rm self.result
rm ref.result
rm compare
echo "===running(Ric)==="
./ref/cirTest-ref -f do1 2> ref.result 1>&2
echo "===running(Kevin)==="
./cirTest -f do1 2> self.result 1>&2
echo "===diff======"
diff self.result ref.result | tee compare
