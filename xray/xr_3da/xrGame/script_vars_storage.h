////////////////////////////////////////////////////////////////////////////
//	Module 		: script_vars_storage.h
//	Created 	: 19.10.2014
//  Modified 	: 21.10.2014
//	Author		: Alexander Petrov
//	Description : global script vars class, with saving content to savegame
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "script_export_space.h"
#include "../../build_config_defines.h"



#define		LUA_TNETPACKET		0x00000100
#define		SVT_ARRAY_TABLE		0x00020000    // ������������ ������ ��������� �������
#define		SVT_ARRAY_ZEROK		0x00040000    // ������������� ������ � ������� ��������
#define		SVT_KEY_BOOLEAN		0x00100000   
#define		SVT_KEY_NUMERIC		0x00300000	  
#define		SVT_KEY_MASK		0x00F00000

#define		SVT_ALLOCATED	    0x10000000


class NET_Packet;
class CScriptVarsTable;

typedef struct _SCRIPT_VAR
{
   // string32	   name;
   int		   type;     // tag 
   u32		   size;
   union {
	   void*					data;
	   double					n_value;
	   bool						b_value;
	   char						s_value[8];
	   CScriptVarsTable			*T;
	   NET_Packet				*P;
   };

   u32							eff_type				() { return type & 0xffff; }
   void							release					();
   void*						smart_alloc				(int new_type, u32 cb = 4);   

   bool							is_key_boolean          () { return (type & SVT_KEY_MASK) == SVT_KEY_BOOLEAN; }
   bool							is_key_numeric          () { return (type & SVT_KEY_MASK) == SVT_KEY_NUMERIC; }

} SCRIPT_VAR;

typedef xr_map<shared_str, SCRIPT_VAR> SCRIPT_VARS_MAP;

class CScriptVarsTable // ������� ����������
{
private:
	
	SCRIPT_VARS_MAP			    m_map;
public:

	bool					    is_array;
	bool						zero_key;

	virtual ~CScriptVarsTable ();

	virtual void				clear					();
	ICF SCRIPT_VARS_MAP&		map						() { return m_map; }
	virtual void				load					(IReader  &memory_stream);
	virtual void				save					(IWriter  &memory_stream);
			void				get						(lua_State *L, LPCSTR k, bool unpack);
			void				set						(lua_State *L, int key_index, int value_index);
			void				set						(lua_State *L, LPCSTR k, int index, int key_type);
	ICF		int					size					() { return map().size(); };
};

class CScriptVarsStorage:
	public CScriptVarsTable
{
	typedef CScriptVarsTable	inherited;

public:
	
	virtual void				load					(IReader  &memory_stream);
	virtual void				save					(IWriter  &memory_stream);
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

#ifdef  SCRIPT_VARS_STORAGE
add_to_type_list(CScriptVarsStorage)
#undef script_type_list
#define script_type_list save_type_list(CScriptVarsStorage)
#endif

extern CScriptVarsStorage g_ScriptVars;
extern void lua_pushsvt(lua_State *L, CScriptVarsTable *T);
