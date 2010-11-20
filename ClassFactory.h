#ifndef _ClassFactory_h_
#define _ClassFactory_h_

#include <Draw/Draw.h>

NAMESPACE_UPP;

template<class T> class WithFactory
{
	private:
		typedef One<T> (*CreateFunc)();
		typedef VectorMap<String, CreateFunc>mapType;
		static mapType &classMap() { static mapType cMap; return cMap; }
		static VectorMap<String, String> &typeMap() { static VectorMap<String, String> tMap; return tMap; }
		static VectorMap<String, String> &descMap() { static VectorMap<String, String> dMap; return dMap; }
		static VectorMap<String, dword> &indexMap() { static VectorMap<String, dword> iMap; return iMap; }
		static VectorMap<String, String> &imageMap() { static VectorMap<String, String> imgMap; return imgMap; }

		static VectorMap<dword, String> &groupDescMap() { static VectorMap<dword, String> grpDescMap; return grpDescMap; }
		static VectorMap<dword, String> &groupIconMap() { static VectorMap<dword, String> grpIcnMap; return grpIcnMap; }
		template<class D> static One<T> __Create(void) { return One<T>((T *)new D); }
	public:
	
		template<class D> static void Register(const String &name, const String &desc = "", dword idx = 0, String const &img = "")
		{
			classMap().Add(name, __Create<D>);
			typeMap().Add(typeid(D).name(), name);
			descMap().Add(name, desc);
			indexMap().Add(name, idx);
			imageMap().Add(name, img);
		}
		static One<T> CreateInstance(const String &className) { return classMap().Get(className)(); }
		static T *CreatePtr(String const &className) { return classMap().Get(className)().Detach(); }
		static Vector<String> const &Classes(void) { return classMap().GetKeys(); }
		static String GetClassDescription(const String &className) { return GetLngString(descMap().Get(className)); }
		static dword const &GetClassIndex(const String &className) { return indexMap().Get(className); }
		static Image GetClassImage(const String &className)
		{
			String const &imlName = imageMap().Get(className);
			if(imlName == "")
				return Null;
			else
				return GetImlImage(imlName);
		}
		String const &IsA(void) { return typeMap().Get(typeid(*this).name()); }
		
		// class groups handling
		static void RegisterClassGroupIcon(dword gMask, String const &desc, String const &img) { groupIconMap().FindAdd(gMask, img); groupDescMap().FindAdd(gMask, desc); }
		static Image GetGroupIcon(dword gMask)
		{
			int idx = groupIconMap().Find(gMask);
			if(idx != -1)
			{
				String const &imlName = groupIconMap().operator[](idx);
				if(imlName == "")
					return Null;
				else
					return GetImlImage(imlName);
			}
			else
				return Null;
		}
		static String GetGroupDesc(dword gMask)
		{
			int idx = groupDescMap().Find(gMask);
			if(idx != -1)
				return GetLngString(groupDescMap().operator[](idx));
			else
				return "";
		}
		static Vector<dword>GetGroupIndexes(dword groupMask, dword forceMask = 0, dword forceVal = 0)
		{
			Index<dword> idx;
			Vector<dword> allIdx;
			allIdx <<= groupDescMap().GetKeys();
			for(int i = 0; i < allIdx.GetCount(); i++)
			{
				dword ci = allIdx.operator[](i);
				if( ((ci & forceMask) == (forceVal & forceMask)) && (ci & groupMask))
					idx.FindAdd((ci & groupMask) | (forceMask & forceVal));
			}
			Vector<dword>res = idx.PickKeys();
			Sort(res);
			return res;
		}
		static Vector<String>GetGroupClasses(dword gMask)
		{
			Vector<String>names;
			Vector<dword>indexes;
			VectorMap<String, dword> const &iMap = indexMap();
			for(int i = 0; i < iMap.GetCount(); i++)
			{
				dword ci = iMap.operator[](i);
				if((gMask & ci) == gMask)
				{
					names.Add(iMap.GetKey(i));
					indexes.Add(ci);
				}
			}
			IndexSort(indexes, names);
			return names;
		}
		
		virtual ~WithFactory() {}
};

#ifdef COMPILER_MSC
#define REGISTERCLASS(type, ...) \
	INITBLOCK { \
		type::Register<type>(#type, __VA_ARGS__); \
}
#else
#define REGISTERCLASS(type, ...) \
	INITBLOCK { \
		type::Register<type>(#type, ##__VA_ARGS__); \
}
#endif

#define REGISTERCLASSGROUPICON(type, groupMask, desc, icon) \
	INITBLOCK { \
		type::RegisterClassGroupIcon(groupMask, desc, icon); \
	}
END_UPP_NAMESPACE;

#endif
