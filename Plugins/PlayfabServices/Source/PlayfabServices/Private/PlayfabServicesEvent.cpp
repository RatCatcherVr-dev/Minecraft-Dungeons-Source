#include "PlayfabServicesEvent.h"
#include "UObject/Class.h"
#include "UObject/NameTypes.h"
#include "UObject/UnrealType.h"

PlayfabServicesEvent::PlayfabServicesEvent(const FString& userId, const FString& eventName)
    : mUserId(userId)
    , mName(eventName)
    , mShouldAggregate(false)
    , mAggregationTimeSeconds(60U)
    , mProperties()
    , mMeasurements()
    , mProcessedRealtime(false)
	{}

PlayfabServicesEvent::PlayfabServicesEvent()
	: mShouldAggregate(false)
	, mAggregationTimeSeconds(60U)
	, mProperties()
	, mMeasurements()
	, mProcessedRealtime(false)
{}

void PlayfabServicesEvent::addProperty(const PlayfabServicesProperty& prop) {
    mProperties.emplace(prop.getName(), prop);
}

void PlayfabServicesEvent::addProperty(const FString& propertyName, const bool& value) {
    PlayfabServicesProperty prop(propertyName, value);
    mProperties.emplace(propertyName, prop);
}

void PlayfabServicesEvent::addProperty(const FString& propertyName, const float& value) {
    PlayfabServicesProperty prop(propertyName, value);
    mProperties.emplace(propertyName, prop);
}

void PlayfabServicesEvent::addProperty(const FString& propertyName, const int32_t& value) {
    PlayfabServicesProperty prop(propertyName, value);
    mProperties.emplace(propertyName, prop);
}

void PlayfabServicesEvent::addProperty(const FString& propertyName, const uint32_t& value) {
    PlayfabServicesProperty prop(propertyName, value);
    mProperties.emplace(propertyName, prop);
}

void PlayfabServicesEvent::addProperty(const FString& propertyName, const char* value) {
    addProperty(propertyName, FString { value });
}

void PlayfabServicesEvent::addProperty(const FString& propertyName, const FString& value) {
    PlayfabServicesProperty prop(propertyName, value);
    mProperties.emplace(propertyName, prop);
}

void PlayfabServicesEvent::addVectorProperties(const FString& propertyBaseName, const FVector& vectorValue)
{
    PlayfabServicesProperty propX(propertyBaseName + ".X", vectorValue.X);
    PlayfabServicesProperty propY(propertyBaseName + ".Y", vectorValue.Y);
    PlayfabServicesProperty propZ(propertyBaseName + ".Z", vectorValue.Z);

    mProperties.emplace(propX.getName(), propX);
    mProperties.emplace(propY.getName(), propY);
    mProperties.emplace(propZ.getName(), propZ);
}

void PlayfabServicesEvent::addEnumProperty(const FString& propertyName, const UEnum* enumType, const uint8_t& value)
{
    if (!enumType) {
        return;
    }

    FString enumVal = enumType->GetNameStringByIndex(static_cast<int32>(value));
    PlayfabServicesProperty p(propertyName, enumVal);
    mProperties.emplace(propertyName, p);
}

void PlayfabServicesEvent::addObjectUProperties(const FString& propertyBaseName, const UObject* objectInstance, bool recursive)
{
    std::set<const UObject *> circularGuard;
    addUObjectPropertiesRecursive(propertyBaseName, objectInstance, circularGuard, recursive);
}

void PlayfabServicesEvent::addMeasurement(const PlayfabServicesMeasurement& measure) {
    mMeasurements.emplace(measure.getName(), measure);
}

void PlayfabServicesEvent::addMeasurement(const FString& measureName, PlayfabServicesMeasurement::AggregationType aggregationType, const bool& value) {
    PlayfabServicesMeasurement measure(measureName, aggregationType, value);
    mMeasurements.emplace(measureName, measure);
}

void PlayfabServicesEvent::addMeasurement(const FString& measureName, PlayfabServicesMeasurement::AggregationType aggregationType, const float& value) {
    PlayfabServicesMeasurement measure(measureName, aggregationType, value);
    mMeasurements.emplace(measureName, measure);
}

void PlayfabServicesEvent::addMeasurement(const FString& measureName, PlayfabServicesMeasurement::AggregationType aggregationType, const int32_t& value) {
    PlayfabServicesMeasurement measure(measureName, aggregationType, value);
    mMeasurements.emplace(measureName, measure);
}

void PlayfabServicesEvent::addMeasurement(const FString& measureName, PlayfabServicesMeasurement::AggregationType aggregationType, const uint32_t& value) {
    PlayfabServicesMeasurement measure(measureName, aggregationType, value);
    mMeasurements.emplace(measureName, measure);
}

const FString& PlayfabServicesEvent::getName() const {
    return mName;
}

void PlayfabServicesEvent::setShouldAggregate(bool value) {
    mShouldAggregate = value;
}

bool PlayfabServicesEvent::getShouldAggregate() const {
    return mShouldAggregate;
}

uint32_t PlayfabServicesEvent::getAggregationTime() const {
    return mAggregationTimeSeconds;
}

void PlayfabServicesEvent::setAggregationTime(uint32_t seconds) {
    mAggregationTimeSeconds = seconds;
}

bool PlayfabServicesEvent::getProcessedRealtime() const {
    return mProcessedRealtime;
}

void PlayfabServicesEvent::setProcessedRealtime(bool processedRealtime) {
    mProcessedRealtime = processedRealtime;
}

const FString& PlayfabServicesEvent::getUserId() const {
    return mUserId;
}

const PlayfabServicesPropertyList& PlayfabServicesEvent::getProperties() const {
    return mProperties;
}

const PlayfabServicesMeasurementList& PlayfabServicesEvent::getMeasurements() const {
    return mMeasurements;
}

void PlayfabServicesEvent::addUObjectPropertiesRecursive(const FString& propertyBaseName, const UObject* objectInstance, std::set<const UObject*>& circularGuard, bool recursive)
{
    if (!objectInstance) {
        return;
    }

    UClass* objClass = objectInstance->GetClass();
    if (!objClass) {
        return;
    }

    for (TFieldIterator<UProperty> propIt(objClass); propIt; ++propIt) {
        UProperty* prop = *propIt;
        if (!prop) {
            continue;
        }

        FString prpName = prop->GetName();
        FString propName = propertyBaseName + "." + TCHAR_TO_UTF8(*prpName);
        UClass* propClass = prop->GetClass();
        if (propClass == UBoolProperty::StaticClass()) {
            UBoolProperty* boolProp = Cast<UBoolProperty>(prop);
            if (boolProp) {
                PlayfabServicesProperty p(propName, boolProp->GetPropertyValue_InContainer(objectInstance));
                mProperties.emplace(propName, p);
            }
        }
        else if (propClass == UFloatProperty::StaticClass()) {
            UFloatProperty* floatProp = Cast<UFloatProperty>(prop);
            if (floatProp) {
                PlayfabServicesProperty p(propName, floatProp->GetPropertyValue_InContainer(objectInstance));
                mProperties.emplace(propName, p);
            }
        }
        else if (propClass == UUInt16Property::StaticClass()) {
            UUInt16Property* intProp = Cast<UUInt16Property>(prop);
            if (intProp) {
                PlayfabServicesProperty p(propName, static_cast<uint32_t>(intProp->GetPropertyValue_InContainer(objectInstance)));
                mProperties.emplace(propName, p);
            }
        }
        else if (propClass == UUInt32Property::StaticClass()) {
            UUInt32Property* intProp = Cast<UUInt32Property>(prop);
            if (intProp) {
                PlayfabServicesProperty p(propName, intProp->GetPropertyValue_InContainer(objectInstance));
                mProperties.emplace(propName, p);
            }
        }
        else if (propClass == UUInt64Property::StaticClass()) {
            UUInt64Property* intProp = Cast<UUInt64Property>(prop);
            if (intProp) {
                PlayfabServicesProperty p(propName, static_cast<uint32_t>(intProp->GetPropertyValue_InContainer(objectInstance)));
                mProperties.emplace(propName, p);
            }
        }
        else if (propClass == UIntProperty::StaticClass()) {
            UIntProperty* intProp = Cast<UIntProperty>(prop);
            if (intProp) {
                PlayfabServicesProperty p(propName, intProp->GetPropertyValue_InContainer(objectInstance));
                mProperties.emplace(propName, p);
            }
        }
        else if (propClass == UInt8Property::StaticClass()) {
            UInt8Property* intProp = Cast<UInt8Property>(prop);
            if (intProp) {
                PlayfabServicesProperty p(propName, static_cast<int32_t>(intProp->GetPropertyValue_InContainer(objectInstance)));
                mProperties.emplace(propName, p);
            }
        }
        else if (propClass == UInt64Property::StaticClass()) {
            UInt64Property* intProp = Cast<UInt64Property>(prop);
            if (intProp) {
                PlayfabServicesProperty p(propName, static_cast<int32_t>(intProp->GetPropertyValue_InContainer(objectInstance)));
                mProperties.emplace(propName, p);
            }
        }
        else if (propClass == UDoubleProperty::StaticClass()) {
            UDoubleProperty* doubleProp = Cast<UDoubleProperty>(prop);
            if (doubleProp) {
                PlayfabServicesProperty p(propName, static_cast<float>(doubleProp->GetPropertyValue_InContainer(objectInstance)));
                mProperties.emplace(propName, p);
            }
        }
        else if (propClass == UByteProperty::StaticClass()) {
            UByteProperty* byteProp = Cast<UByteProperty>(prop);
            if (byteProp) {
                uint8_t byteVal = byteProp->GetPropertyValue_InContainer(objectInstance);

                UEnum* enumType = byteProp->GetIntPropertyEnum();
                if (enumType) {
                    addEnumProperty(propName, enumType, byteVal);
                }
                else {
                    PlayfabServicesProperty p(propName, static_cast<uint32_t>(byteVal));
                    mProperties.emplace(propName, p);
                }
            }
        }
        else if (propClass == UStructProperty::StaticClass()) {
            UStructProperty* structProp = Cast<UStructProperty>(prop);
            if (structProp) {
                UScriptStruct* structVal = structProp->Struct;
                if (structVal == TBaseStructure<FVector>::Get()) {
                    FVector vecVal;
                    structProp->CopyCompleteValue(&vecVal, objectInstance);
                    addVectorProperties(propName, vecVal);
                }
            }
        }
        else if (recursive && propClass == UObjectProperty::StaticClass()) {
            UObjectProperty* objProp = Cast<UObjectProperty>(prop);
            if (objProp) {
                UObject* newObjInstance = objProp->GetObjectPropertyValue_InContainer(objectInstance);
                if (circularGuard.find(newObjInstance) == circularGuard.end()) {
                    continue;
                }
                
                circularGuard.insert(newObjInstance);

                FString objPropName = objProp->GetName();
                FString deepPropName = propName + "." + TCHAR_TO_UTF8(*objPropName);
                addUObjectPropertiesRecursive(deepPropName, newObjInstance, circularGuard, true);
            }
        }
    }
}
