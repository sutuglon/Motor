
#include <string>
#include <vector>
#include "vec.h"
#include "chrono.h"


#ifdef WIN32
LARGE_INTEGER Chronometer::m_freq;
#else
#endif

Chronometer::Chronometer (const std::string& label)
	:m_label(label),
	m_lapse(0)
{;}

void Chronometer::init() {
#ifdef WIN32
	QueryPerformanceFrequency(&m_freq);
#else

#endif
}

void Chronometer::reset() {
    m_lapse = 0;
    m_counter = 0;
}

void Chronometer::start() {
#ifdef WIN32    
	QueryPerformanceCounter(&m_start);
#else
    gettimeofday(&m_start, 0);
#endif
}

void Chronometer::stop()
{
    m_counter = 1;
#ifdef WIN32
	QueryPerformanceCounter(&m_end);
	m_lapse = 
		(real)((real)(m_end.QuadPart - m_start.QuadPart) / (real)m_freq.QuadPart) *1000;
#else
    gettimeofday(&m_end, 0);
    m_lapse =
        (m_end.tv_sec - m_start.tv_sec)*static_cast<real> (1e03) + (m_end.tv_usec - m_start.tv_usec)*static_cast<real>(1e-03);
#endif
}

void Chronometer::lapse()
{
#ifdef WIN32
	QueryPerformanceCounter(&m_end);
	m_lapse += 
         (real)((real)(m_end.QuadPart - m_start.QuadPart) / (real)m_freq.QuadPart) *1000;
#else
    m_lapse +=
        (m_end.tv_sec - m_start.tv_sec)*static_cast<real> (1e03) + (m_end.tv_usec - m_start.tv_usec)*static_cast<real>(1e-03);         
#endif
   ++m_counter;
}

//void Chronometer::output(class view* view, int x, int y)
//{
//   view->print(x, y, m_label +  rg::itoa(m_counter)+ " " +rg::ftoa(m_lapse) );
//}

Chronos& Chronos::Singleton() 
{
	static Chronos singleton;
	return singleton;
}

Chronos::~Chronos()
{
	for(unsigned i=0;i<m_Swatches.size();++i) {
		delete m_Swatches[i];
	}
}

Chronometer* Chronos::add(const std::string& label)
{
	Chronometer* ret = new Chronometer(label);
	m_Swatches.push_back (ret);
	return ret;
}

Chronometer* Chronos::get(const std::string& label)
{
	Chronometer* ret = 0;
	unsigned i=0;
	while(i<m_Swatches.size() && m_Swatches[i]->m_label != label) {
		i++;
	}
	if (i<m_Swatches.size()) {
		ret = m_Swatches[i];
	}
	return ret;
}

//void Chronos::Output(class view* view, rgUint16 x, rgUint16 y)
//{
//	for(unsigned i=0;i<m_Swatches.size();++i) {
//		m_Swatches[i]->output(view, x, y+=10);
//	}
//}