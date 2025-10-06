//
// Generated file, do not edit! Created by opp_msgtool 6.0 from openflow/messages/ControllerCommand.msg.
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
#include "ControllerCommand_m.h"

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

namespace openflow {

Register_Class(ControllerCommand)

ControllerCommand::ControllerCommand(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

ControllerCommand::ControllerCommand(const ControllerCommand& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

ControllerCommand::~ControllerCommand()
{
    delete [] this->vehicleIDs;
}

ControllerCommand& ControllerCommand::operator=(const ControllerCommand& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void ControllerCommand::copy(const ControllerCommand& other)
{
    this->cmd = other.cmd;
    this->targetRSU = other.targetRSU;
    delete [] this->vehicleIDs;
    this->vehicleIDs = (other.vehicleIDs_arraysize==0) ? nullptr : new int[other.vehicleIDs_arraysize];
    vehicleIDs_arraysize = other.vehicleIDs_arraysize;
    for (size_t i = 0; i < vehicleIDs_arraysize; i++) {
        this->vehicleIDs[i] = other.vehicleIDs[i];
    }
}

void ControllerCommand::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->cmd);
    doParsimPacking(b,this->targetRSU);
    b->pack(vehicleIDs_arraysize);
    doParsimArrayPacking(b,this->vehicleIDs,vehicleIDs_arraysize);
}

void ControllerCommand::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->cmd);
    doParsimUnpacking(b,this->targetRSU);
    delete [] this->vehicleIDs;
    b->unpack(vehicleIDs_arraysize);
    if (vehicleIDs_arraysize == 0) {
        this->vehicleIDs = nullptr;
    } else {
        this->vehicleIDs = new int[vehicleIDs_arraysize];
        doParsimArrayUnpacking(b,this->vehicleIDs,vehicleIDs_arraysize);
    }
}

const char * ControllerCommand::getCmd() const
{
    return this->cmd.c_str();
}

void ControllerCommand::setCmd(const char * cmd)
{
    this->cmd = cmd;
}

const char * ControllerCommand::getTargetRSU() const
{
    return this->targetRSU.c_str();
}

void ControllerCommand::setTargetRSU(const char * targetRSU)
{
    this->targetRSU = targetRSU;
}

size_t ControllerCommand::getVehicleIDsArraySize() const
{
    return vehicleIDs_arraysize;
}

int ControllerCommand::getVehicleIDs(size_t k) const
{
    if (k >= vehicleIDs_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)vehicleIDs_arraysize, (unsigned long)k);
    return this->vehicleIDs[k];
}

void ControllerCommand::setVehicleIDsArraySize(size_t newSize)
{
    int *vehicleIDs2 = (newSize==0) ? nullptr : new int[newSize];
    size_t minSize = vehicleIDs_arraysize < newSize ? vehicleIDs_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        vehicleIDs2[i] = this->vehicleIDs[i];
    for (size_t i = minSize; i < newSize; i++)
        vehicleIDs2[i] = 0;
    delete [] this->vehicleIDs;
    this->vehicleIDs = vehicleIDs2;
    vehicleIDs_arraysize = newSize;
}

void ControllerCommand::setVehicleIDs(size_t k, int vehicleIDs)
{
    if (k >= vehicleIDs_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)vehicleIDs_arraysize, (unsigned long)k);
    this->vehicleIDs[k] = vehicleIDs;
}

void ControllerCommand::insertVehicleIDs(size_t k, int vehicleIDs)
{
    if (k > vehicleIDs_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)vehicleIDs_arraysize, (unsigned long)k);
    size_t newSize = vehicleIDs_arraysize + 1;
    int *vehicleIDs2 = new int[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        vehicleIDs2[i] = this->vehicleIDs[i];
    vehicleIDs2[k] = vehicleIDs;
    for (i = k + 1; i < newSize; i++)
        vehicleIDs2[i] = this->vehicleIDs[i-1];
    delete [] this->vehicleIDs;
    this->vehicleIDs = vehicleIDs2;
    vehicleIDs_arraysize = newSize;
}

void ControllerCommand::appendVehicleIDs(int vehicleIDs)
{
    insertVehicleIDs(vehicleIDs_arraysize, vehicleIDs);
}

void ControllerCommand::eraseVehicleIDs(size_t k)
{
    if (k >= vehicleIDs_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)vehicleIDs_arraysize, (unsigned long)k);
    size_t newSize = vehicleIDs_arraysize - 1;
    int *vehicleIDs2 = (newSize == 0) ? nullptr : new int[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        vehicleIDs2[i] = this->vehicleIDs[i];
    for (i = k; i < newSize; i++)
        vehicleIDs2[i] = this->vehicleIDs[i+1];
    delete [] this->vehicleIDs;
    this->vehicleIDs = vehicleIDs2;
    vehicleIDs_arraysize = newSize;
}

class ControllerCommandDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_cmd,
        FIELD_targetRSU,
        FIELD_vehicleIDs,
    };
  public:
    ControllerCommandDescriptor();
    virtual ~ControllerCommandDescriptor();

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

Register_ClassDescriptor(ControllerCommandDescriptor)

ControllerCommandDescriptor::ControllerCommandDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(openflow::ControllerCommand)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

ControllerCommandDescriptor::~ControllerCommandDescriptor()
{
    delete[] propertyNames;
}

bool ControllerCommandDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ControllerCommand *>(obj)!=nullptr;
}

const char **ControllerCommandDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *ControllerCommandDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int ControllerCommandDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int ControllerCommandDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_cmd
        FD_ISEDITABLE,    // FIELD_targetRSU
        FD_ISARRAY | FD_ISEDITABLE | FD_ISRESIZABLE,    // FIELD_vehicleIDs
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *ControllerCommandDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "cmd",
        "targetRSU",
        "vehicleIDs",
    };
    return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int ControllerCommandDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "cmd") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "targetRSU") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "vehicleIDs") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *ControllerCommandDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_cmd
        "string",    // FIELD_targetRSU
        "int",    // FIELD_vehicleIDs
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **ControllerCommandDescriptor::getFieldPropertyNames(int field) const
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

const char *ControllerCommandDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int ControllerCommandDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    ControllerCommand *pp = omnetpp::fromAnyPtr<ControllerCommand>(object); (void)pp;
    switch (field) {
        case FIELD_vehicleIDs: return pp->getVehicleIDsArraySize();
        default: return 0;
    }
}

void ControllerCommandDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    ControllerCommand *pp = omnetpp::fromAnyPtr<ControllerCommand>(object); (void)pp;
    switch (field) {
        case FIELD_vehicleIDs: pp->setVehicleIDsArraySize(size); break;
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'ControllerCommand'", field);
    }
}

const char *ControllerCommandDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    ControllerCommand *pp = omnetpp::fromAnyPtr<ControllerCommand>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ControllerCommandDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    ControllerCommand *pp = omnetpp::fromAnyPtr<ControllerCommand>(object); (void)pp;
    switch (field) {
        case FIELD_cmd: return oppstring2string(pp->getCmd());
        case FIELD_targetRSU: return oppstring2string(pp->getTargetRSU());
        case FIELD_vehicleIDs: return long2string(pp->getVehicleIDs(i));
        default: return "";
    }
}

void ControllerCommandDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ControllerCommand *pp = omnetpp::fromAnyPtr<ControllerCommand>(object); (void)pp;
    switch (field) {
        case FIELD_cmd: pp->setCmd((value)); break;
        case FIELD_targetRSU: pp->setTargetRSU((value)); break;
        case FIELD_vehicleIDs: pp->setVehicleIDs(i,string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ControllerCommand'", field);
    }
}

omnetpp::cValue ControllerCommandDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    ControllerCommand *pp = omnetpp::fromAnyPtr<ControllerCommand>(object); (void)pp;
    switch (field) {
        case FIELD_cmd: return pp->getCmd();
        case FIELD_targetRSU: return pp->getTargetRSU();
        case FIELD_vehicleIDs: return pp->getVehicleIDs(i);
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'ControllerCommand' as cValue -- field index out of range?", field);
    }
}

void ControllerCommandDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ControllerCommand *pp = omnetpp::fromAnyPtr<ControllerCommand>(object); (void)pp;
    switch (field) {
        case FIELD_cmd: pp->setCmd(value.stringValue()); break;
        case FIELD_targetRSU: pp->setTargetRSU(value.stringValue()); break;
        case FIELD_vehicleIDs: pp->setVehicleIDs(i,omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ControllerCommand'", field);
    }
}

const char *ControllerCommandDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr ControllerCommandDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    ControllerCommand *pp = omnetpp::fromAnyPtr<ControllerCommand>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void ControllerCommandDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    ControllerCommand *pp = omnetpp::fromAnyPtr<ControllerCommand>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ControllerCommand'", field);
    }
}

}  // namespace openflow

namespace omnetpp {

}  // namespace omnetpp

