#ifndef SINGLETON_H
#define SINGLETON_H

template <class Product>
struct DefaultSingletonCreator
{
    static Product *create() { return new Product; }
};

template <class Product>
struct NullSingletonCreator
{
    static Product *create() { return 0; }
};

template 
<
    class T,
    template <class> class Creator = DefaultSingletonCreator
>
class Singleton
{
public:
    static T &self()
    {
        if ( !s_self )
            s_self = Creator<T>::create();
        return *s_self;
    }

protected:
    Singleton()
    { s_self = static_cast<T *>( this ); }
    ~Singleton()
    { s_self = 0; }

private:
    Singleton( const Singleton<T, Creator> &rhs );
    Singleton<T, Creator> &operator=( const Singleton<T, Creator> &rhs );

    static T *s_self;
};

template <class T, template <class> class Creator>
T *Singleton<T, Creator>::s_self = 0;

#endif // SINGLETON_H
/* vim: et sw=4 ts=4
 */
