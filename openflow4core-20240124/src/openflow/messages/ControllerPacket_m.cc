//
// Generated file, do not edit! Created by opp_msgtool 6.0 from openflow/messages/ControllerPacket.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "ControllerPacket_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Class(ControllerPacket)

ControllerPacket::ControllerPacket(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

ControllerPacket::ControllerPacket(const ControllerPacket& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

ControllerPacket::~ControllerPacket()
{
}

ControllerPacket& ControllerPacket::operator=(const ControllerPacket& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void ControllerPacket::copy(const ControllerPacket& other)
{
    this->originalVehicleMessage = other.originalVehicleMessage;
    this->rsuId = other.rsuId;
    this->sourceVehicleId = other.sourceVehicleId;
    this->destinationVehicleId = other.destinationVehicleId;
    this->controllerInstruction = other.controllerInstruction;
    this->targetRsuId = other.targetRsuId;
}

void ControllerPacket::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->originalVehicleMessage);
    doParsimPacking(b,this->rsuId);
    doParsimPacking(b,this->sourceVehicleId);
    doParsimPacking(b,this->destinationVehicleId);
    doParsimPacking(b,this->controllerInstruction);
    doParsimPacking(b,this->targetRsuId);
}

void ControllerPacket::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->originalVehicleMessage);
    doParsimUnpacking(b,this->rsuId);
    doParsimUnpacking(b,this->sourceVehicleId);
    doParsimUnpacking(b,this->destinationVehicleId);
    doParsimUnpacking(b,this->controllerInstruction);
    doParsimUnpacking(b,this->targetRsuId);
}

const omnetpp::cPacket * ControllerPacket::getOriginalVehicleMessage() const
{
    return this->originalVehicleMessage;
}

void ControllerPacket::setOriginalVehicleMessage(omnetpp::cPacket * originalVehicleMessage)
{
    this->originalVehicleMessage = originalVehicleMessage;
}

const char * ControllerPacket::getRsuId() const
{
    return this->rsuId.c_str();
}

void ControllerPacket::setRsuId(const char * rsuId)
{
    this->rsuId = rsuId;
}

const char * ControllerPacket::getSourceVehicleId() const
{
    return this->sourceVehicleId.c_str();
}

void ControllerPacket::setSourceVehicleId(const char * sourceVehicleId)
{
    this->sourceVehicleId = sourceVehicleId;
}

const char * ControllerPacket::getDestinationVehicleId() const
{
    return this->destinationVehicleId.c_str();
}

void ControllerPacket::setDestinationVehicleId(const char * destinationVehicleId)
{
    this->destinationVehicleId = destinationVehicleId;
}

const char * ControllerPacket::getControllerInstruction() const
{
    return this->controllerInstruction.c_str();
}

void ControllerPacket::setControllerInstruction(const char * controllerInstruction)
{
    this->controllerInstruction = controllerInstruction;
}

const char * ControllerPacket::getTargetRsuId() const
{
    return this->targetRsuId.c_str();
}

void ControllerPacket::setTargetRsuId(const char * targetRsuId)
{
    this->targetRsuId = targetRsuId;
}

class ControllerPacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_originalVehicleMessage,
        FIELD_rsuId,
        FIELD_sourceVehicleId,
        FIELD_destinationVehicleId,
        FIELD_controllerInstruction,
        FIELD_targetRsuId,
    };
  public:
    ControllerPacketDescriptor();
    virtual ~ControllerPacketDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(ControllerPacketDescriptor)

ControllerPacketDescriptor::ControllerPacketDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(ControllerPacket)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

ControllerPacketDescriptor::~ControllerPacketDescriptor()
{
    delete[] propertyNames;
}

bool ControllerPacketDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ControllerPacket *>(obj)!=nullptr;
}

const char **ControllerPacketDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *ControllerPacketDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int ControllerPacketDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 6+base->getFieldCount() : 6;
}

unsigned int ControllerPacketDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISCOMPOUND | FD_ISPOINTER | FD_ISCOBJECT | FD_ISCOWNEDOBJECT | FD_ISREPLACEABLE,    // FIELD_originalVehicleMessage
        FD_ISEDITABLE,    // FIELD_rsuId
        FD_ISEDITABLE,    // FIELD_sourceVehicleId
        FD_ISEDITABLE,    // FIELD_destinationVehicleId
        FD_ISEDITABLE,    // FIELD_controllerInstruction
        FD_ISEDITABLE,    // FIELD_targetRsuId
    };
    return (field >= 0 && field < 6) ? fieldTypeFlags[field] : 0;
}

const char *ControllerPacketDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "originalVehicleMessage",
        "rsuId",
        "sourceVehicleId",
        "destinationVehicleId",
        "controllerInstruction",
        "targetRsuId",
    };
    return (field >= 0 && field < 6) ? fieldNames[field] : nullptr;
}

int ControllerPacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "originalVehicleMessage") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "rsuId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "sourceVehicleId") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "destinationVehicleId") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "controllerInstruction") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "targetRsuId") == 0) return baseIndex + 5;
    return base ? base->findField(fieldName) : -1;
}

const char *ControllerPacketDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "omnetpp::cPacket",    // FIELD_originalVehicleMessage
        "string",    // FIELD_rsuId
        "string",    // FIELD_sourceVehicleId
        "string",    // FIELD_destinationVehicleId
        "string",    // FIELD_controllerInstruction
        "string",    // FIELD_targetRsuId
    };
    return (field >= 0 && field < 6) ? fieldTypeStrings[field] : nullptr;
}

const char **ControllerPacketDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *ControllerPacketDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int ControllerPacketDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    ControllerPacket *pp = omnetpp::fromAnyPtr<ControllerPacket>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void ControllerPacketDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    ControllerPacket *pp = omnetpp::fromAnyPtr<ControllerPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'ControllerPacket'", field);
    }
}

const char *ControllerPacketDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    ControllerPacket *pp = omnetpp::fromAnyPtr<ControllerPacket>(object); (void)pp;
    switch (field) {
        case FIELD_originalVehicleMessage: { const omnetpp::cPacket * value = pp->getOriginalVehicleMessage(); return omnetpp::opp_typename(typeid(*value)); }
        default: return nullptr;
    }
}

std::string ControllerPacketDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    ControllerPacket *pp = omnetpp::fromAnyPtr<ControllerPacket>(object); (void)pp;
    switch (field) {
        case FIELD_originalVehicleMessage: { auto obj = pp->getOriginalVehicleMessage(); return obj == nullptr ? "" : obj->str(); }
        case FIELD_rsuId: return oppstring2string(pp->getRsuId());
        case FIELD_sourceVehicleId: return oppstring2string(pp->getSourceVehicleId());
        case FIELD_destinationVehicleId: return oppstring2string(pp->getDestinationVehicleId());
        case FIELD_controllerInstruction: return oppstring2string(pp->getControllerInstruction());
        case FIELD_targetRsuId: return oppstring2string(pp->getTargetRsuId());
        default: return "";
    }
}

void ControllerPacketDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ControllerPacket *pp = omnetpp::fromAnyPtr<ControllerPacket>(object); (void)pp;
    switch (field) {
        case FIELD_rsuId: pp->setRsuId((value)); break;
        case FIELD_sourceVehicleId: pp->setSourceVehicleId((value)); break;
        case FIELD_destinationVehicleId: pp->setDestinationVehicleId((value)); break;
        case FIELD_controllerInstruction: pp->setControllerInstruction((value)); break;
        case FIELD_targetRsuId: pp->setTargetRsuId((value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ControllerPacket'", field);
    }
}

omnetpp::cValue ControllerPacketDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    ControllerPacket *pp = omnetpp::fromAnyPtr<ControllerPacket>(object); (void)pp;
    switch (field) {
        case FIELD_originalVehicleMessage: return omnetpp::toAnyPtr(pp->getOriginalVehicleMessage()); break;
        case FIELD_rsuId: return pp->getRsuId();
        case FIELD_sourceVehicleId: return pp->getSourceVehicleId();
        case FIELD_destinationVehicleId: return pp->getDestinationVehicleId();
        case FIELD_controllerInstruction: return pp->getControllerInstruction();
        case FIELD_targetRsuId: return pp->getTargetRsuId();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'ControllerPacket' as cValue -- field index out of range?", field);
    }
}

void ControllerPacketDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ControllerPacket *pp = omnetpp::fromAnyPtr<ControllerPacket>(object); (void)pp;
    switch (field) {
        case FIELD_originalVehicleMessage: pp->setOriginalVehicleMessage(omnetpp::fromAnyPtr<omnetpp::cPacket>(value.pointerValue())); break;
        case FIELD_rsuId: pp->setRsuId(value.stringValue()); break;
        case FIELD_sourceVehicleId: pp->setSourceVehicleId(value.stringValue()); break;
        case FIELD_destinationVehicleId: pp->setDestinationVehicleId(value.stringValue()); break;
        case FIELD_controllerInstruction: pp->setControllerInstruction(value.stringValue()); break;
        case FIELD_targetRsuId: pp->setTargetRsuId(value.stringValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ControllerPacket'", field);
    }
}

const char *ControllerPacketDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_originalVehicleMessage: return omnetpp::opp_typename(typeid(omnetpp::cPacket));
        default: return nullptr;
    };
}

omnetpp::any_ptr ControllerPacketDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    ControllerPacket *pp = omnetpp::fromAnyPtr<ControllerPacket>(object); (void)pp;
    switch (field) {
        case FIELD_originalVehicleMessage: return omnetpp::toAnyPtr(pp->getOriginalVehicleMessage()); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void ControllerPacketDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    ControllerPacket *pp = omnetpp::fromAnyPtr<ControllerPacket>(object); (void)pp;
    switch (field) {
        case FIELD_originalVehicleMessage: pp->setOriginalVehicleMessage(omnetpp::fromAnyPtr<omnetpp::cPacket>(ptr)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ControllerPacket'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

