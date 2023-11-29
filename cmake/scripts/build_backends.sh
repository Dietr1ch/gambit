backends_list_file=$1

# Check that the input file exists
if [ ! -f ${backends_list_file} ];
then
  printf "%s\n" "Cannot find the file ${backends_list_file}" >&2
  exit 1
fi

# For each backend listed in default_backends.txt: 
# - Try to build it
# - If the build fails, add a line to failed_backends_file
did_fail=0
failed_backends_list=""
while read backend || [[ -n $backend ]];
do
  make ${backend}
  if [ $? -ne 0 ];
  then 
    did_fail=1
    failed_backends_list="${failed_backends_list}, ${backend}"
  fi
done < ${backends_list_file}

failed_backends_list="${failed_backends_list:2}"

if [ ${did_fail} -eq 1 ];
then
  printf "%s\n" "The following backends failed to build: ${failed_backends_list}" >&2
else
  printf "%s\n" "All backends built successfully!"
fi

exit ${did_fail}
