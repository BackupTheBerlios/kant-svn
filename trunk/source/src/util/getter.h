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
#ifndef XETA_GETTER_H
#define XETA_GETTER_H

// FIXME - Sollen Objekte new und deleten für temporäre Objekte? -- Con: permanentes Erzeugen und Freigeben, stetige init Kosten verlangt kopierkonstruktor - viele deletes
// sollen properties eigene pod Daten haben dürfen? -- Con: permanentes vorhandensein von Daten pro Klasse, unsaubere Objekte, verlangt op=
// Soll ein Allokator verwendet werden? Zum Beschleunigen bzw. 
// als generelle Option 
//
// obige Probleme sind durch sinnvolle get - set Methoden nebensache

// const T& com_
// const T*
template <typename T, typename Compound, typename MemFuncPtr>
struct getter_method_ref : public getter<T,Compound>
{
	MemFuncPtr func;
	getter_method_ref( MemFuncPtr f ) : func(f) {}
	T const& get_ref( const Compound* obj )
	{ return (obj->*func)(); }
};

template <typename T, typename Compound, typename MemFuncPtr>
struct getter_method_ptr : public getter<T,Compound>
{
	MemFuncPtr func;
	getter_method_ptr( MemFuncPtr f ) : func(f) {}
	T const& get_ref( const Compound* obj )
	{ return *(obj->*func)(); }
};

template <typename T, typename Compound, typename MemFuncPtr>
struct getter_method_ptr_freeable : public getter<T,Compound>
{
	MemFuncPtr func;
	T const* ptr;
	
	getter_method_ptr_freeable( MemFuncPtr f ) : func(f), ptr(0) {}
	
	void acquire( const Compound* obj )
	{ delete ptr; ptr = (obj->*func)(obj); }
	
	T const& get_ref( const Compound* obj )
	{ return *ptr; }
	void release()
	{ delete ptr;  ptr = 0; }
	~getter_method_ptr_freeable()
	{ delete ptr; }
};

// ret by value..
template <typename T, typename Compound, typename MemFuncPtr>
struct getter_method_value : public getter<T,Compound>
{
	MemFuncPtr func;
	T* element;
	getter_method_value( MemFuncPtr f ) : func(f), element(0) {}

	void acquire( const Compound* obj )
	{ delete element; element = new T((obj->*func)()); }
	
	T const& get_ref( const Compound* obj )
	{ return *element; }
	
	void release( const Compound* obj )
	{ delete element; element = 0; }

	~getter_method_value( )
	{ delete element; }
};

// returnvalue within the parameter:
// _ignore_ Compound::*( [const] T[*|&] ret ) const; 
template <typename T, typename Compound, typename MemFuncPtr>
struct getter_method_param_ref : public getter<T,Compound>
{
	MemFuncPtr func;
	T *element;
	getter_method_param_ref( MemFuncPtr f ) : func(f), element(0) {}
	void acquire( const Compound* obj )
	{
		delete element;
		element = new T; 
		(obj->*func)( *element );
	}
	T const& get_ref( const Compound* obj )
	{ return *element; }

	void release( const Compound* obj )
	{
		delete element;
		element = 0;
	}
	~getter_method_param_ref() { delete element; }
};

template <typename T, typename Compound, typename MemFuncPtr>
struct getter_method_param_ptr : public getter<T,Compound>
{
	MemFuncPtr func;
	T *element;
	getter_method_param_ptr( MemFuncPtr f ) : func(f), element(0) {}
	void acquire( const Compound* obj )
	{
		delete element;
		element = new T; 
		(obj->*func)( element );
	}
	T const& get_ref( const Compound* obj )
	{ return *element; }

	void release( const Compound* obj )
	{
		delete element;
		element = 0;
	}
	~getter_method_param_ptr() { delete element; }
};
// reference to the object
// general functors  - mostly like above

// get object by memory offset
template <typename T, typename Compound>
struct getter_offset : public getter<T,Compound>
{
	member_offset off;
	getter_offset( member_offset o ) : off(o) {}
	T const & get_ref( const Compound* obj )
	{ return off.template get_const_ref<T>(obj); }
};

template< typename T, typename Compound>
boost::shared_ptr<getter<T,Compound> > gen_get( const T& (Compound::*method)() const )
{ return boost::shared_ptr<getter<T,Compound> >( new getter_method_ref<T,Compound,const T& (Compound::*)() const>( method ) ); }

template< typename T, typename Compound>
boost::shared_ptr<getter<T,Compound> > gen_get( T& (Compound::*method)() const )
{ return boost::shared_ptr<getter<T,Compound> >( new getter_method_ref<T,Compound,T& (Compound::*)() const>( method ) ); }

template< typename T, typename Compound>
boost::shared_ptr<getter<T,Compound> > gen_get( T* (Compound::*method)() const )
{ return boost::shared_ptr<getter<T,Compound> >( new getter_method_ptr<T,Compound,T* (Compound::*)() const>( method ) ); }

template< typename T, typename Compound>
boost::shared_ptr<getter<T,Compound> > gen_get( const T* (Compound::*method)() const )
{ return boost::shared_ptr<getter<T,Compound> >( new getter_method_ptr<T,Compound,const T* (Compound::*)() const>( method ) ); }

template< typename T, typename Compound>
boost::shared_ptr<getter<T,Compound> > gen_get( T (Compound::*method)() const )
{ return boost::shared_ptr<getter<T,Compound> >( new getter_method_value<T,Compound,T (Compound::*)() const>( method ) ); }

template<typename T, typename Compound, typename QuietReturn>
boost::shared_ptr<getter<T,Compound> > gen_get( QuietReturn (Compound::*method)( T& ) const )
{ return boost::shared_ptr<getter<T,Compound> >( new getter_method_param_ref <T,Compound,QuietReturn (Compound::*)( T& ) const>( method ) ); }

template<typename T, typename Compound, typename QuietReturn>
boost::shared_ptr<getter<T,Compound> > gen_get( QuietReturn (Compound::*method)( T* ) const )
{ return boost::shared_ptr<getter<T,Compound> >( new getter_method_param_ref <T,Compound,QuietReturn (Compound::*)( T* ) const>( method ) ); }

template<typename T, typename Compound>
boost::shared_ptr<getter<T,Compound> > gen_get( const member_offset & offset )
{ return boost::shared_ptr<getter<T,Compound> >( new getter_offset<T,Compound>( offset ) ); }

#endif

