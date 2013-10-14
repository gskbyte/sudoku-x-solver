#include <iostream>
#include <cstring>

using namespace std;

class SudokuCell
{
friend class Sudoku;

public:
    SudokuCell();
    bool solved() const
        { return value != 0; }
    int getValue() const
        { return value; }
    bool validValue(int val) const
        { return marked[val-1] == 0; }
protected:
    bool mark(int n);
    int setValue();
    void markPeers();
    
    void testMarkPeers();
    void unmark(int n);
    void setTestValue(int val);
    void unmarkPeers();
protected:
    int value;

    /* 
    *  Indica valores marcados
    *  Usado principalmente para detectar valores válidos de celda
    */
    bool peers_marked;
    int marked[9];
    int n_marked;
    
    SudokuCell * column_peers[8];
    SudokuCell * row_peers[8];
    SudokuCell * square_peers[4];
    SudokuCell * diagonal_peers[16];
    int n_diagonal_peers;
};

class Sudoku
{
public:
    Sudoku();
    int solvedCells() const
        { return n_solved_cells; }
    bool solved() const
        { return solvedCells() == 81; }
    
    void load(char * numbers);
    bool solve();
    
    void print() const;
    bool isOk();
    void getStats();
protected:
    void nextCell(int & i, int & j);
    void nextEmptyCell(int & i, int & j);
    
    void setPeers();
    void generateEmptyCellsList();
    SudokuCell * getMostRestrictedCell();
    bool backtrackingSolving(SudokuCell * current);
protected:
    SudokuCell cells[9][9];
    int number_frequency[9];
    int n_solved_cells;
    
    SudokuCell * empty_cells[81];
    int n_empty_cells;
};

/// Métodos de las clases

SudokuCell::SudokuCell()
{
    peers_marked = false;
    for(int i=0; i<9; ++i)
        marked[i]=0;
    n_diagonal_peers = 0;
}

inline bool SudokuCell::mark(int n)
{
    int index;
    index = n-1;
    if(marked[index]==0)
        ++n_marked;
    ++marked[index];
    
    return n_marked == 8;
}

int SudokuCell::setValue()
{
    if(n_marked == 8)
    {
        for(int i=0; i<9; ++i)
            if(marked[i]==0){
                value=i+1;
                continue;
            }
        return value; 
    }
    else
        return 0;
}

void SudokuCell::markPeers()
{
    if( !solved() || peers_marked )
        return;
    peers_marked = true;
    
    // Mark column & row
    for(int i=0; i<8; ++i){
        if(!column_peers[i]->solved())
            if(column_peers[i]->mark(value)){
                column_peers[i]->setValue();
                column_peers[i]->markPeers();
            }
        if(!row_peers[i]->solved())
            if(row_peers[i]->mark(value)){
                row_peers[i]->setValue();
                row_peers[i]->markPeers();
        }
    }
    
    // Mark square
    for(int i=0; i<4; ++i)
        if(!square_peers[i]->solved())
            if(square_peers[i]->mark(value)){
                square_peers[i]->setValue();
                square_peers[i]->markPeers();
            }
    
    // Mark diagonals
    for(int i=0; i<n_diagonal_peers; ++i)
        if(!diagonal_peers[i]->solved())
            if(diagonal_peers[i]->mark(value)){
                diagonal_peers[i]->setValue();
                diagonal_peers[i]->markPeers();
            }
}

void SudokuCell::testMarkPeers()
{
    // Mark column & row
    for(int i=0; i<8; ++i){
        column_peers[i]->mark(value);
        row_peers[i]->mark(value);
    }
    
    // Mark square
    for(int i=0; i<4; ++i)
        square_peers[i]->mark(value);
    
    // Mark diagonals
    for(int i=0; i<n_diagonal_peers; ++i)
        diagonal_peers[i]->mark(value);
}

inline void SudokuCell::unmark(int n)
{
    --marked[n-1];
    
    if(marked[n-1]==0)
        --n_marked;
}

void SudokuCell::setTestValue(int val)
{
    if(val != 0){
        value = val;
        ++marked[value-1];
    }
    else{
        marked[value-1] = 0;
        value = val;
    }
}

inline void SudokuCell::unmarkPeers()
{
    // Unmark column & row
    for(int i=0; i<8; ++i){
        column_peers[i]->unmark(value);
        row_peers[i]->unmark(value);
    }
    // Unmark square
    for(int i=0; i<4; ++i)
        square_peers[i]->unmark(value);
    
    // Unmark diagonals
    for(int i=0; i<n_diagonal_peers; ++i)
        diagonal_peers[i]->unmark(value);
}

Sudoku::Sudoku()
{
    n_solved_cells = 0;
    for(int i=0; i<9; ++i)
        number_frequency[i]=0;
    n_empty_cells = 0;
}

void Sudoku::load(char * numbers)
{
    int index;
    SudokuCell * cell;
    bool ok;
    int i, j;
    
    index = 0;
    // Para cada celda leo el valor
    for(int i=0; i<9; ++i){
        for(int j=0; j<9; ++j, ++index){
            // 1. Inserción de valores
            cell = &cells[i][j];
            if(numbers[index] >= '1' && numbers[index] <= '9'){
                cell->value = numbers[index] - '0';
                ++cell->marked[numbers[index] - '0' - 1];
                cell->n_marked = 1;
            }
            else{
                if(numbers[index] == '.' || numbers[index] == '-' ||
                   numbers[index] == '0'){
                    cell->value = 0;
                    cell->n_marked = 0;
                }
                else
                    --j;
            }
        }
    }
    
    // Establezco los vecinos de cada celda y hago los marcados i
    setPeers();
}

bool Sudoku::solve()
{
    bool success = false;
    if( this->isOk() ){
        // 1. Marcados iniciales
        for(int i=0; i<9; ++i)
            for(int j=0; j<9; ++j)
                cells[i][j].markPeers();
        
        if( solved() )
            success = true;
        else{
            // 2. Resolución mediante backtracking
            generateEmptyCellsList();
            success = backtrackingSolving(getMostRestrictedCell());
        }
    }
    return success;
}

void Sudoku::print() const
{
    for(int i=0; i<9; ++i)
        for(int j=0; j<9; ++j)
            cout << cells[i][j].value;
    cout << "\n";
}

bool Sudoku::isOk(){
    SudokuCell * current;
    int value;
    // Para cada celda, ver que es distinta a sus vecinas
    for(int i=0; i<9; ++i){
        for(int j=0; j<9; ++j){
            current = &cells[i][j];
            value = current->getValue();
            if(value != 0){
                // 1. Filas y columnas
                for(int k=0; k<8; ++k){
                    if(current->column_peers[k]->value==value)
                        return false;
                    if(current->row_peers[k]->value==value)
                        return false;
                }
                // 2. Cuadrado
                for(int k=0; k<4; ++k)
                    if(current->square_peers[k]->value==value)
                        return false;
                // 3. Diagonal(es)
                for(int k=0; k<current->n_diagonal_peers; ++k)
                    if(current->diagonal_peers[k]->value==value)
                        return false;
            }
        }
    }
    return true;
}

inline void Sudoku::nextCell(int & i, int & j)
{
    if(j<8)
        ++j;
    else // j == 8
        if(i<8){
            ++i;
            j=0;
        }
        else // error (límite sobrepasado)
            i = j = -1;
}

inline void Sudoku::nextEmptyCell(int & i, int & j)
{
    bool cont = true;
    nextCell(i,j);
    
    while(cont){
        if(i == -1)
            cont = false;
        else
            if(cells[i][j].solved()) // si está resuelta (=> no vacía)
                nextCell(i, j);
            else
                cont = false;
    }
}

void Sudoku::setPeers()
{
    int ind_row, ind_col, ind_sq, ind_diag; // para navegar sobre vecinos
    int sqx, sqy; // indican cuadrado al que pertenece la celda
    
    SudokuCell * current;
    
    for(int i=0; i<9; ++i){
        for(int j=0; j<9; ++j){
            current = &cells[i][j];
            // 2. Inserción de vecinos
            // 2.1. Fila y columna
            ind_row = ind_col = 0;
            for(int k=0; k<9; ++k){
                if(k!=i){
                    current->column_peers[ind_col] = &cells[k][j];
                    ++ind_col;
                }
                if(k!=j){
                    current->row_peers[ind_row] = &cells[i][k];
                    ++ind_row;
                }
            }
            
            // 2.3. Cuadrado
            // determino el cuadrado
            // Fullero pero rápido
            // La buena: sqy = i - i%3;
            //           sqx = j - j%3:
            if(i<3) sqy = 0;
            else if(i<6) sqy = 3;
            else sqy = 6;
            
            if(j<3) sqx = 0;
            else if(j<6) sqx = 3;
            else sqx = 6;
            
            ind_sq=0;
            for(int y=sqy; y<sqy+3; ++y)
                for(int x=sqx; x<sqx+3; ++x)
                    if(y!=i && x!=j){
                        current->square_peers[ind_sq] = &cells[y][x];
                        ++ind_sq;
                    }
            
            // 2.4 Diagonales
            // 2.4.1. ¿Está en diagonal principal?
            if(i == j){
                ind_diag = 0;
                for(int d=0; d<9; ++d)
                    if(d != i){ // si no es ella misma
                        current->diagonal_peers[ind_diag] = &cells[d][d];
                        ++ind_diag;
                    }
                current->n_diagonal_peers += 8;
            }
            
            // 2.4.1. ¿Está en diagonal secundaria?
            if(i == 8 - j){
                ind_diag = 0;
                for(int d=0; d<9; ++d)
                    if(d != i){ // si no es ella misma
                        current->diagonal_peers[ind_diag + current->n_diagonal_peers] = 
                            &cells[d][8-d];
                        ++ind_diag;
                    }
                current->n_diagonal_peers += 8;
            }
        }
    }
}

void Sudoku::getStats()
{
    for(int i=0; i<9; ++i)
        for(int j=0; j<9; ++j)
            if(cells[i][j].solved()){
                ++n_solved_cells;
                ++number_frequency[ cells[i][j].getValue()-1 ];
            }
}

void Sudoku::generateEmptyCellsList()
{
    SudokuCell * current = NULL;
    
    int y, x;
    y = 0; x = -1;
    
    nextEmptyCell(y, x);
    while(y != -1){
        empty_cells[n_empty_cells] = &cells[y][x];
        ++n_empty_cells;
        nextEmptyCell(y, x);
    }
}

SudokuCell * Sudoku::getMostRestrictedCell()
{
    SudokuCell * ret = NULL;
    int n_marked = -1;
    for(int i=0; i<n_empty_cells; ++i){
        if(empty_cells[i]->n_marked > n_marked &&
           !empty_cells[i]->solved()){
            n_marked = empty_cells[i]->n_marked;
            ret = empty_cells[i];
        }
    }
    return ret;
}

bool Sudoku::backtrackingSolving(SudokuCell * current)
{
    if(current == NULL)
        return true;
    
    int test_value = 1;
    bool result = false;
    
    // Pruebo todos los valores
    while(test_value <=9){
        if( current->validValue(test_value) ){
            current->setTestValue(test_value);
            current->testMarkPeers();
            result = backtrackingSolving(getMostRestrictedCell());
            if( !result ){
                current->unmarkPeers();
                current->setTestValue(0);
            }
        }
        ++test_value;
    }
    
    return result;
}

int main(int argc, char ** argv)
{
    Sudoku s;
    if(argc != 2)
        cout << "Uso: \n\tsudoku <cadena_numeros>" << endl;
    else{
        s.load(argv[1]);
        if( s.solve() )
            s.print();
    }
}