/***************************************************************************
 *   Copyright (C) 2004 by Andreas Pokorny <apokorny@zynot.org>            *
 *                                                                         *
 * This file is part of the Xeta package manager.                          *
 *                                                                         *
 * Xeta is free software; you can redistribute it and/or modify            *
 * it under the terms of the GNU Lesser General Public License as          *
 * published by the Free Software Foundation; either version 2 of the      *
 * License, or (at your option) any later version.                         *
 *                                                                         *
 * Xeta is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this software; if not, write to the Free Software            *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.               *
 ***************************************************************************/
#ifndef XETA_SETTER_H
#define XETA_SETTER_H
template <typename T, typename Compound, typename MemFuncPtr>
struct setter_method_ref : public setter<T,Compound>
{
	T* element;
	MemFuncPtr func;
	setter_method_ref( MemFuncPtr fun  ) : element(0), func(fun) {}
	void acquire( Compound* obj )
	{
		delete element;
		element = new T;
	}
	T& get_ref(Compound* obj) { return *element; }
	void release( Compound* obj )
	{
	//	std::cout << __FILE__ << ":" <<__LINE__ << " " << obj << " "<< element << std::endl;
		(obj->*func)(*element);
		delete element;
		element = 0;
	}
	~setter_method_ref()
	{
		delete element;
	}

};

template <typename T, typename Compound, typename MemFuncPtr>
struct setter_method_ptr : public setter<T,Compound>
{
	T* element;
	MemFuncPtr func;
	setter_method_ptr( MemFuncPtr fun  ) : element(0), func(fun) {}
	void acquire( Compound* obj )
	{
		delete element;
		element = new T;
	}
	T& get_ref(Compound* obj) { return *element; }
	void release( Compound* obj )
	{
		(obj->func)(element);
		delete element;
		element = 0;
	}
	~setter_method_ptr()
	{
		delete element;
	}

};

template <typename T, typename Functor>
struct setter_functor_ref : public setter<T>
{
	T* element;
	Functor func;	
	setter_functor_ref( Functor fun  ) : element(0), func(fun) {}
	void acquire( mpl::void_* obj )
	{
		delete element;
		element = new T;
	}
	T& get_ref( mpl::void_* obj) { return *element; }
	void release( mpl::void_* obj )
	{
		func(*element);
		delete element;
		element = 0;
	}
	~setter_functor_ref()
	{
		delete element;
	}
};
template <typename T, typename Functor>
struct setter_functor_ptr : public setter<T>
{
	T* element;
	Functor func;	
	setter_functor_ptr( Functor fun  ) : element(0), func(fun) {}
	void acquire( mpl::void_* obj )
	{
		delete element;
		element = new T;
	}
	T& get_ref( mpl::void_* obj) { return *element; }
	void release( mpl::void_* obj )
	{
		func(element);
		delete element;
		element = 0;
	}
	~setter_functor_ptr()
	{
		delete element;
	}
};

template <typename T>
struct setter_ref : public setter<T>
{
	T& ref;
	setter_ref( T &r ) : ref(r) {}

	T& get_ref( mpl::void_* obj) {
		return ref;
	}
	~setter_ref( );

};

template <typename T, typename Compound >
struct setter_offset : public setter<T,Compound>
{
	member_offset element;
	setter_offset( member_offset e ) : element(e) {}

	T& get_ref( Compound* obj ) {
		
		return element . template get_ref<T>(obj);
	}
	~setter_offset() { }
};

template <typename T, typename Compound, typename MethodPtr>
struct setter_dma_ptr: public setter<T,Compound>
{
	MethodPtr func;
	setter_dma_ptr( MethodPtr f ) : func(f) {}

	T& get_ref( Compound* obj) {
		return *(obj->*func)();
	}
	~setter_dma_ptr() { }
};

template <typename T, typename Compound, typename MethodPtr>
struct setter_dma_ref : public setter<T,Compound>
{
	MethodPtr func;
	setter_dma_ref( MethodPtr f ) : func(f) {}

	T& get_ref( Compound* obj) {
		return (obj->*func)();
	}
	~setter_dma_ref() { }
};

template <typename T, typename Functor>
struct setter_dma_ref_functor : public setter<T>
{
	Functor func;
	setter_dma_ref_functor( Functor f ) : func(f) {}

	T& get_ref( mpl::void_* obj) 
	{
		return func();
	}
	~setter_dma_ref_functor() { }
};

template <typename T, typename Functor>
struct setter_dma_ptr_functor : public setter<T>
{
	Functor func;
	setter_dma_ptr_functor( Functor f ) : func(f) {}

	T& get_ref( mpl::void_* obj) {
		return *func();
	}
	~setter_dma_ptr_functor() { }
};

// Generator for member offsets:
template<typename T, typename Compound>
boost::shared_ptr<setter<T, Compound> > gen_set( member_offset offset )
{
	return boost::shared_ptr<setter<T, Compound> >( 
			new setter_offset<T,Compound>( offset ) 
			);
}

// Generators for method pointers
template<typename T, typename Compound>
boost::shared_ptr<setter<T, Compound> > gen_set( void (Compound::*method)( T const & ) )
{
	return boost::shared_ptr<setter<T, Compound> >( 
			new setter_method_ref<T,Compound,void(Compound::*)( T const & ) >(method ) );
}

template<typename T, typename Compound>
boost::shared_ptr<setter<T, Compound> > gen_set( void (Compound::*method)( T ) )
{
	return boost::shared_ptr<setter<T, Compound> >( 
			new setter_method_ref<T,Compound,void(Compound::*)( T ) >(method ) );
}


template<typename T, typename Compound>
boost::shared_ptr<setter<T, Compound> > gen_set( void (Compound::*method)( T const * ) )
{
	return boost::shared_ptr<setter<T, Compound> >( 
			new setter_method_ptr<T,Compound,void(Compound::*)( T const * ) >(method ) );
}

template<typename T, typename Compound>
boost::shared_ptr<setter<T,Compound> > gen_set( T* (Compound::*method)() )
{
	return boost::shared_ptr<setter<T, Compound> >( 
			new setter_dma_ptr<T,Compound, T* (Compound::*)() >(method) );
}

template<typename T, typename Compound>
boost::shared_ptr<setter<T,Compound> > gen_set( T& (Compound::*method)() )
{
	return boost::shared_ptr<setter<T, Compound> >( 
			new setter_dma_ref<T,Compound, T& (Compound::*)()>(method) );
}

template<typename T, typename Compound>
boost::shared_ptr<setter<T, Compound> > gen_set( void (Compound::*method)( T const & ) const )
{
	return boost::shared_ptr<setter<T, Compound> >( 
			new setter_method_ref<T,Compound,void(Compound::*)( T const & ) const >(method ) );
}

template<typename T, typename Compound>
boost::shared_ptr<setter<T, Compound> > gen_set( void (Compound::*method)( T ) const )
{
	return boost::shared_ptr<setter<T, Compound> >( 
			new setter_method_ref<T,Compound,void(Compound::*)( T ) const >(method ) );
}

/*template<typename T, typename Compound>
boost::shared_ptr<setter<T, Compound> > gen_set( void (Compound::*method)( T const * ) )
{
	return boost::shared_ptr<setter<T, Compound> >( 
			new setter_method_ptr<T,Compound,void(Compound::*)( T const * ) const>(method ) );
}*/

template<typename T, typename Compound>
boost::shared_ptr<setter<T,Compound> > gen_set( T* (Compound::*method)() const )
{
	return boost::shared_ptr<setter<T, Compound> >( 
			new setter_dma_ptr<T,Compound, T* (Compound::*)() const >(method) );
}

template<typename T, typename Compound>
boost::shared_ptr<setter<T,Compound> > gen_set( T& (Compound::*method)() const )
{
	return boost::shared_ptr<setter<T, Compound> >( 
			new setter_dma_ref<T,Compound, T* (Compound::*)() const >(method) );
}

// TODO Generators for Functors: differ between retvalue and parameter based setter 
// -> generating setter directly might be easier 
//
// TODO Some methods return booleans or ints to return info to the user
// for these kinds of methods Generators could be easily written
#endif

