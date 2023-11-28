#!/bin/bash

char="$1"
ok=0 

if [ "$#" -ne 1 ]; then # Verifica faptul ca a primt exact un argument
    echo "Nr incorect arg"  # mesajul de eroare
    exit 1
fi

function linieOK 
{
    linie="$1" #atribuie primul arg al functiei ($1) variabilei locale (linie)
    
    if [[ ! "$linie" =~ ^[A-Z] ]]; then return 1  #vedem daca prima litera e mare si returnam 1
    fi

    if [[ "$linie" =~ $char &&  #verifica daca valoarea variabilei 'linie' contine caracterul stocat in variabila '$char'
          ! "$linie" =~ ,.*[[:space:]]si &&   #verifica daca 'linie' nu contine virgula, spatiu, inainte de si
          "$linie" =~ ^[A-Za-z0-9\ ,.!]+[?!.,]$ ]];   #verifica daca 'linie' incepe cu litere, cifre sau alte caractere si se termina cu . ? ! 
          then return 0  #daca e ok returnam 0
    fi

    return 1  #daca nu e ok returnam 1
}


#Script-ul va citi continuu linii de la intrarea standard pana la intalnirea end-of-file
#și va contoriza numărul de “propoziții corecte” care conțin caracterul primit ca și argument.
while IFS= read -r linie || [ -n "$linie" ]; do
    if linieOK "$linie"; then ((ok++))  #apelam functia de mai sus pt a verifica daca linia este ok si incrementam contorul 'ok'
    fi
done

echo $ok  #afisarea contorului pe ecran