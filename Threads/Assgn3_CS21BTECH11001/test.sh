IFS=' '
read -r line < input.txt
read -ra params <<< $line
echo "${params[@]}"

echo "${params[2]}" "${params[3]}"