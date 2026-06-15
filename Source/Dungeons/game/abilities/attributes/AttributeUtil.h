#pragma once



//Macro source, I've put it in a separate AttributeMacros.h file
#define DECLARE_ATTRIBUTE_FUNCTION(PropertyName) static const FGameplayAttribute& PropertyName##Attribute()


#define DEFINE_ATTRIBUTE_REPLICATION_FUNCTION(PropertyName, ClassName) 	\
void ClassName::OnRep_##PropertyName()								\
{																		\
	GAMEPLAYATTRIBUTE_REPNOTIFY(ClassName, PropertyName);				\
}


#define DEFINE_ATTRIBUTE_FUNCTION(PropertyName, ClassName) 																						\
const FGameplayAttribute& ClassName::PropertyName##Attribute()																					\
{																																				\
	static FGameplayAttribute attibute(FindFieldChecked<UProperty>(ClassName::StaticClass(), GET_MEMBER_NAME_CHECKED(ClassName, PropertyName))); \
	return attibute;																												\
}