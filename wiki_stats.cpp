#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>      // std::setprecision
#include <queue>          // std::queue
#include <map>          // std::map
#include <list>         // std::list
#include <time.h>       // start and finish time
using namespace std;

class WikiGraph {
    public:
    vector<string> _titles; // массив из названий статей
    vector<int> _sizes; // размер статьи
    vector<int> _links; // все статьи, на которые ссылается текущая статья
    vector<bool> _redirect; // является ли эта статья синонимом
    vector<int> _offset; // смещения
    WikiGraph() {	}	

    // загрузка из указанного файла
    void load_from_file(char * filename) {
        char s[300]; // ДЛЯ ОГРОМНОГО ФАЙЛА ЕЩЕ УВЕЛИЧИТЬ (UNICODE использует несколько байт на символ)
        int n, _nlinks;
        int size_bytes, is_redir, outgo_link_no;
        fstream f; 

        cout << "LOADING FROM FILE: " << filename  << endl;
        f.open(filename, ios::in);
        f.seekg(0);
        if ( f.good() ) { // если есть возможность читать из файла
            f >> n >> _nlinks;         //(n, _nlinks) = map(int, f.readline().split()) 
			                           // # прочитать из файла кол-во статей n и кол-во ссылок _nlinks
            cout << "Статей: "<< n << "    Ссылок: " << _nlinks  << endl; // print('Статей',n,'   Ссылок',_nlinks)
            // _titles заполняет push_back()           // self._titles = []
            _sizes.resize(n);            // self._sizes = array.array('L', [0]*n)
            _links.resize(_nlinks);      // self._links = array.array('L', [0]*_nlinks)
            _redirect.resize(n);         //self._redirect = array.array('B', [0]*n)
            _offset.resize(n+1);         //self._offset = array.array('L', [0]*(n+1))

            _offset[0] = 0;             //self._offset[0] = 0 # вначале нужно поставить 0
            int total_loaded_links = 0; // будем подсчитывать кол-во для поддержания массива _offset
            for (int i=0; i<n; i++) {   // загружаем статью номер i
                f >> s;                 //s = f.readline() # построчная загрузка - буду бережно расходовать память
                _titles.push_back(s);   // self._titles.append(s.strip()) # загружаем название i статьи
                f >> size_bytes >> is_redir >> outgo_link_no; // (size_bytes, is_redir, outgo_link_no) = map(int,s.split()) # размер в байтах, флаг перенаправ, кол-во ссылок
                _sizes[i] = size_bytes;   // self._sizes[i] = size_bytes;
                _redirect[i] = is_redir;  // self._redirect[i] = is_redir;
                for (int j=0;j<outgo_link_no;j++) { // зарузим все статьи, на которые ссылается текущая статья
                    int tmp_int;
                    f >> tmp_int;                             // s = f.readline()
                    _links[total_loaded_links + j] = tmp_int; // self._links[total_loaded_links + j] = int(s.strip())
                } // end for j
                total_loaded_links += outgo_link_no;
                _offset[i+1] = total_loaded_links; // # кол-во статей, на которые ссылается текущая (i-ая), вычисля-
                                               // # естся по формуле _offset[i+1] - _offset[i]
	    } // end for i
    	} // end if f.good()
    	f.close();
        cout << "GRAPH LOADED." << endl;
    } // end of load_from_file()
	
    int get_number_of_links_from(int _id) {
        return _offset[_id+1] - _offset[_id]; //return self._offset[_id+1] - self._offset[_id]
	}
    vector<int> get_links_from(int _id) { // нужно получить срез из вектора
        return vector<int> ( &_links[_offset[_id]], &_links[_offset[_id+1]] ) ;  // return self._links[self._offset[_id]:self._offset[_id+1]]
	}
	
    int get_id(string title) { // по строке наименования нужно найти id - ЭТО ПОЛНЫЙ ПЕРЕБОР, но вызывается ТОЛЬКО 2 РАЗА при поиске в ширину
	for (int i=0; i<_titles.size(); i++)
	    if ( _titles[i].compare(title)==0 )
	        return i; // return self._titles.index(title)
	return (-1);
    }
    int get_number_of_pages() {
        return _titles.size(); //return len(self._titles)
    }
	
    bool is_redirect(int _id) {
        return _redirect[_id]; //return self._redirect[_id]
    }
    string get_title(int _id) {
        return _titles[_id]; // return self._titles[_id]
    }
	
    int get_page_size(int _id) {
        return _sizes[_id]; // return self._sizes[_id]
    }
	
    // воспользуемся структурой <map> для visited и структурой <queue> для очереди q
    map<int,int> BFS(int start, int finish) { // # ф-ция поиска в ширину от узла start до finish
        queue<int> q;
	q.push(start); // пока в очереди id начала
        map<int,int> visited;
	visited[start] = -1; // ключем будет id текущей вершины, значением id родителя (у start нет родителя)
        while (!q.empty()) {
            int v = q.front(); // берем первого из очереди в переменную v
	    q.pop();           // и убираем его оттуда
            if ( v == finish ) // нашли что искали
	        return visited; // нужно не забыть "перевернуть" этот  "путь"
            vector<int> links_from = get_links_from(v);
	    for (int i=0; i< links_from.size(); i++) {// по всем соседям пройдемся
                int j = links_from[i]; // id очередного соседа
                map<int,int>::iterator it = visited.find(j); // поиск возвращает итератор
                if ( it == visited.end() ) { // если итератор после поиска равен итератору конца - то не нашли среди посещенных
                    visited[j] = v; // текущую вершину метим как посещенную и запонимаем ее родителя
                    q.push(j); // добавим в очередь обхода
            	}
            } // end for i
            
    	} // end while
    	map<int,int> empt;
        return empt;
    } // end of BFS()
	
};

int main () {
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    cout << "start time: " << ctime (&rawtime) << endl;

    WikiGraph wg;
	wg.load_from_file((char*)"wiki.txt");

    // # СТАТИСТИКА
    int total = wg.get_number_of_pages();
    // # количество статей с перенаправлением
    int total_pages_with_redirect = 0;
    for (int i=0; i<total;i++) { //total_pages_with_redirect = sum(wg.get_number_of_links_from(i) for i in range(total) if wg.is_redirect(i))
	total_pages_with_redirect += ( wg.is_redirect(i) )? wg.get_number_of_links_from(i) : 0;
    }     // total_pages_with_redirect = std::accumulate(0, total, 0, [](int &ret, int &i) { ret += (wg.is_redirect(i)) ? wg.get_number_of_links_from(i):0; return ret;});
    cout << "статей с перенаправлением: " << total_pages_with_redirect << " (" << std::setprecision(3) << ((double)total_pages_with_redirect/total*100) << "%)"<< endl;
    // print('статей с перенаправлением:', total_pages_with_redirect, '(%.2f'%(total_pages_with_redirect/total*100)+'%)')

    // в отдельный список соберем кол-во ссылок из статьи (без перенаправлений)
    vector<int> arr_links_from;         // arr_links_from = array.array('L') # длина массива неизвестна
    for (int i=0; i<total;i++) {  // arr_links_from.extend( wg.get_number_of_links_from(i) for i in range(total) if not wg.is_redirect(i) )  
	if ( ! wg.is_redirect(i) )              
   	    arr_links_from.push_back( wg.get_number_of_links_from(i) );
    }

    // минимальное кол-во ссылок из статьи
    int min_links_from = *( std::min_element(arr_links_from.begin(), arr_links_from.end()) ); // = min(arr_links_from)
    cout << "минимальное кол-во ссылок из статьи: " << min_links_from << endl; // print('минимальное кол-во ссылок из статьи:', min_links_from)
    // кол-во статей с минимальным кол-вом ссылок
    int pages_with_min_links_from = std::count(arr_links_from.begin(), arr_links_from.end(), min_links_from);  // arr_links_from.count(min_links_from)
    cout << "кол-во статей с минимальным кол-вом ссылок: " << pages_with_min_links_from << endl; // print('кол-во статей с минимальным кол-вом ссылок:', pages_with_min_links_from)
    // максим кол-во ссылок из статьи
    int max_links_from = *( std::max_element(arr_links_from.begin(), arr_links_from.end()) ); // = max(arr_links_from)
     cout << "максимальное кол-во ссылок из статьи: "<< max_links_from << endl;     //print('максимальное кол-во ссылок из статьи:', max_links_from)
    // кол-во статей с максим кол-вом ссылок
    int pages_with_max_links_from = std::count(arr_links_from.begin(), arr_links_from.end(), max_links_from); // = arr_links_from.count(max_links_from)
    cout << "кол-во статей с максимальным кол-вом ссылок:" << pages_with_max_links_from << endl;
    // статья с наибольшим  кол-вом ссылок
    for (int i=0; i<total; i++) { // for i in range(total): # на случай если их несколько
        // ВАЖНО: использовать индекс i списка arr_links_from[i] нельзя, т.к. len(arr_links_from) != total
        if (wg.get_number_of_links_from(i) == max_links_from)   //if wg.get_number_of_links_from(i) == max_links_from:
            cout << "статья с наибольшим  кол-вом ссылок: " << wg.get_title(i) << endl;
    }
    // среднее количество ссылок в статье
    double average_links_from = std::accumulate(arr_links_from.begin(), arr_links_from.end(), 0.0)/arr_links_from.size(); // = statistics.mean(arr_links_from)
    double d = 0.0; // среднеквадр отклонение посчитаем...
    for (int i=0; i<arr_links_from.size(); i++) // сумма квадратов разностей с мат.ожиданием
	 d+= ((double)arr_links_from[i]-average_links_from)*((double)arr_links_from[i]-average_links_from);
    d /= (arr_links_from.size()-1);
    double stdev_links_from = sqrt(d); // = statistics.stdev(arr_links_from) # на Питоне библиотека помогала
    cout << "среднее количество ссылок в статье:" << std::setprecision(4) << average_links_from << "(ср.откл. " << stdev_links_from << "%)" << endl; 

    // в отдельный список соберем кол-во внешних ссылок на статью (использeем array.array)
    // ВАЖНО: перенаправление не считается внешней ссылкой
    // если этот список сформировать без перенаправлений, то не сможем легко вычислить статью с наиб. кол-вом внеш ссылок
    vector<int> incoming_links_no (total, 0); // = array.array('L', [0]*total) # посчитаем для каждой статьи кол-во ссылок на нее
    for (int i=0;i<total;i++) { // i in range(total):
	vector<int> tmp_arr = wg.get_links_from(i);
	if (! wg.is_redirect(i)) { // перенаправление не считается внешней ссылкой
            for (int j=0; j<tmp_arr.size(); j++) // j in wg.get_links_from(i):
                incoming_links_no[tmp_arr[j]] += 1;
    	}
    }
    // минимальное количество ссылок на статью (перенаправление не считается внешней ссылкой)
    int min_links_to = *( std::min_element(incoming_links_no.begin(), incoming_links_no.end()) ); // = min(incoming_links_no)
    cout << "минимальное количество ссылок на статью:" << min_links_to << endl;

    // количество статей с минимальным количеством внешних ссылок
    int pages_with_min_links_to = std::count(incoming_links_no.begin(), incoming_links_no.end(), min_links_to); // = sum(1+0*i for i in range(total) if incoming_links_no[i]==min_links_to )
    cout << "количество статей с минимальным количеством внешних ссылок: " << pages_with_min_links_to << endl;

    // максимальное количество ссылок на статью
    int max_links_to = *( std::max_element(incoming_links_no.begin(), incoming_links_no.end()) ); // max(incoming_links_no)
    cout << "максимальное количество ссылок на статью: " << max_links_to << endl;
    // количество статей с максимальным количеством внешних ссылок
    int pages_with_max_links_to = std::count(incoming_links_no.begin(), incoming_links_no.end(), max_links_to); // = sum(1+i*0 for i in range(total) if incoming_links_no[i]==max_links_to)
    cout << "количество статей с максимальным количеством внешних ссылок: " << pages_with_max_links_to << endl;
    // статья с наибольшим количеством внешних ссылок
    int title_idx =  std::find(incoming_links_no.begin(), incoming_links_no.end(), max_links_to) - incoming_links_no.begin(); 
	       // это индекс максим элемента получен как разность итераторов (итератор найденного - итератор начала)
    cout << "статья с наибольшим количеством внешних ссылок: " << wg.get_title(title_idx) << endl; 
	        //Питон в одну строку:  wg.get_title(incoming_links_no.index(max_links_to) ) )
    // среднее количество внешних ссылок на статью
    double average_links_to = std::accumulate(incoming_links_no.begin(), incoming_links_no.end(), 0.0)/incoming_links_no.size(); 
	       // Питон = statistics.mean(incoming_links_no)
    d = 0.0; // среднеквадр отклонение посчитаем...
    for (int i=0; i<incoming_links_no.size(); i++) // сумма квадратов разностей с мат.ожиданием
	 d+= ((double)incoming_links_no[i]-average_links_to)*((double)incoming_links_no[i]-average_links_to);
    d /= (incoming_links_no.size()-1);
    double stdev_links_to = sqrt(d); // = statistics.stdev(incoming_links_no) # на Питоне библиотека помогала
    cout << "среднее количество внешних ссылок на статью: " << std::setprecision(4) << average_links_to << 
	          "(ср.откл. " << stdev_links_to << "%)" << endl;

    // минимальное количество перенаправлений на статью
    vector<int> external_redirection (total, 0); // = array.array('L', [0]*total) # посчитаем кол-во перенаправлений на каждую статью
    for (int i=0; i<total; i++) {
        if ( wg.is_redirect(i) ) // если вместо статьи перенаправление, то всего одна ссылка
            external_redirection[wg.get_links_from(i)[0]] += 1;
    }
    int min_redirection = *( std::min_element(external_redirection.begin(), external_redirection.end()) ); //= min(external_redirection)
    cout << "минимальное количество перенаправлений на статью: " << min_redirection << endl;
    // количество статей с минимальным количеством внешних перенаправлений
    int pages_with_min_redirect = std::count(external_redirection.begin(), external_redirection.end(), min_redirection); // =external_redirection.count(min_redirection)
    cout << "количество статей с минимальным количеством внешних перенаправлений: " << pages_with_min_redirect << endl;
    // максимальное количество перенаправлений на статью
    int max_redirection = *( std::max_element(external_redirection.begin(), external_redirection.end()) ); //= max(external_redirection)
    cout << "максимальное количество перенаправлений на статью: " << max_redirection << endl;
    // количество статей с максимальным количеством внешних перенаправлений
    int pages_with_max_redirect = std::count(external_redirection.begin(), external_redirection.end(), max_redirection);// = external_redirection.count(max_redirection)
    cout << "количество статей с максиммальным количеством внешних перенаправлений: " << pages_with_max_redirect << endl;
    // статья с наибольшим количеством внешних перенаправлений
    title_idx = std::find(external_redirection.begin(), external_redirection.end(), max_redirection) - external_redirection.begin();
    cout << "статья с наибольшим количеством внешних перенаправлений: " << wg.get_title(title_idx) << endl;
               // На питоне в одну строку ...... wg.get_title( external_redirection.index(max_redirection) ) )
    // среднее количество внешних перенаправлений на статью
    double average_redirect = std::accumulate(external_redirection.begin(), external_redirection.end(), 0.0)/external_redirection.size(); // = statistics.mean(external_redirection)
    d = 0.0; // среднеквадр отклонение посчитаем...
    for (int i=0; i<external_redirection.size(); i++) // сумма квадратов разностей с мат.ожиданием
	 d+= ((double)external_redirection[i]-average_redirect)*((double)external_redirection[i]-average_redirect);
    d /= (external_redirection.size()-1);
    double stdev_redirect = sqrt(d);  // На Питоне в одну строку =statistics.stdev(external_redirection)
    cout << "среднее количество внешних перенаправлений на статью: " << std::setprecision(2) << average_redirect <<
               " (ср.откл. " << stdev_redirect << "%)" << endl;

    // путь, по которому можно добраться от статьи "Python" до статьи "Боль"
    int first_page = wg.get_id(u8"Python");
    int last_page  = wg.get_id(u8"Боль");
    // int last_page  = wg.get_id(u8"Список_файловых_систем");
    
    // воспользуемся поиском в ширину с указанием вершины-цели
    cout << "Запускаем поиск в ширину." << endl;
    map<int,int> revers_path = wg.BFS(first_page,last_page); // получим путь обратно без last_page в виде словаря
    if (!revers_path.empty()) {
        cout << "Поиск закончен. Найден путь:" << endl;
        // из словаря {id потомка : id родителя} нужно распечатать прямой путь
	std::list<int> direct_path;
	std::list<int>::iterator it;
        direct_path.push_back(last_page); // это список из id статей - прямой путь
        int current = last_page;
        while ( revers_path.find(current) != revers_path.end() ) { // значением для ключа first_page является -1:
            current = revers_path[current];
            if (current==-1) // замена None
                break;
            direct_path.insert(direct_path.begin(), current); // вставка спереди
    	}
        for (it=direct_path.begin(); it!=direct_path.end(); ++it)
            cout << wg.get_title(*it) << endl;
    }
    else
        cout << "Поиск закончен. Путь не найден." << endl;
        

    time ( &rawtime );
    cout << "finish time: " << ctime (&rawtime) << endl;

}
