for ((i = 1; i<10; ++i)) do
    echo $i
    game/./Game Dummy Dummy Dummy Harry -s $i < game/default.cnf > game/default.res 
done