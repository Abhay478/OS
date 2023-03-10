echo Output: > output.txt
echo Vary Passengers: > output.txt
echo c,p > q.csv
for n in 10 15 20 25 30 35 40 45 50; 
do 
    echo 25 $n 5 5 5 > inp-params.txt;
    for i in {1..5}; 
    do 
        ./main
        echo $i
    done
done

python3 people.py

echo Vary Cars: >> output.txt
echo c,p > q.csv
for n in 5 10 15 20 25; 
do 
    echo $n 50 5 5 3 > inp-params.txt;
    for i in {1..5}; 
    do 
        ./main
        echo $i
    done
done

python3 cars.py

