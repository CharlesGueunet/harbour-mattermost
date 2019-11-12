#ifndef MATTERMOSTQT_GOBAL_H
#define MATTERMOSTQT_GOBAL_H

#define CONCAT1(a, b) a ## b
#define CONCAT2(a, b) CONCAT1(a, b)

#define CONCAT3(a, b, c) CONCAT2( CONCAT2(a, b), c )

#define MT_PROPERTY_NO_SIGNAL(type_, name_, signal_) \
	private: \
	    Q_PROPERTY( type_ name_ READ name_ WRITE CONCAT2(set_,name_) NOTIFY signal_ ) \
	    type_ CONCAT2(m_,name_); \
	public: \
	    type_ name_ () const \
    { \
	    return CONCAT2(m_,name_); \
	} \
	void CONCAT2(set_,name_) ( type_ value );

#define MT_PROPERTY(type_, name_) \
	MT_PROPERTY_NO_SIGNAL( type_, name_, CONCAT3(on_, name_, Changed) ) \
	Q_SIGNAL void CONCAT3(on_, name_, Changed) ()

#define MT_SET_PROPERTY_SIGNAL(namespace_, type_, name_, signal_ ) \
	void namespace_:: CONCAT2(set_,name_) ( type_ value ) \
    { \
	    CONCAT2(m_,name_) = value; \
	    emit namespace_:: signal_ (); \
	} \

#define MT_SET_PROPERTY(namespace_, type_, name_) \
	MT_SET_PROPERTY_SIGNAL( namespace_, type_, name_, CONCAT3(on_, name_, Changed) )


#define MT_SET_PROPERTY_2(namespace_, type_, name_, add_signal_) \
	void namespace_:: CONCAT2(set_,name_) ( type_ value ) \
    { \
	    CONCAT2(m_,name_) = value; \
	    emit namespace_:: CONCAT3(on_, name_, Changed) (); \
	    emit add_signal_ (); \
	} \


typedef  QVector<int> QVectorInt;

#endif // MATTERMOSTQT_GOBAL_H
