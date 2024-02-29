#include "../include/utils.h"
#include <fstream>
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>

size_t platform_utils::strlen(const char *ptr) {
    const char *s;

    for (s = ptr; *s; ++s);
    return (s - ptr);
}

tp::addr platform_utils::memset(void *_dst, tp::s32 val, size_t size) {
    if ((tp::u64)_dst % sizeof(long) == 0) {
        long *dst = (long *)_dst;

        for (size_t i = 0; i < size; i++)
            *dst++ = val;
    }

    else {
        char *dst = (char *)_dst;

        for (size_t i = 0; i < size; i++)
            *dst++ = (tp::s8)val;
    }
}

tp::addr platform_utils::memcpy(void *_dst, const void *_src, size_t size) {
    if ((tp::u64)_dst % sizeof(long) == 0 &&
        (tp::u64)_src % sizeof(long) == 0) {
        long *dst = (long *)_dst;
        long *src = (long *)_src;

        for (size_t i = 0; i < size; i++)
            *dst++ = *src++;
    }

    else {
        char *dst = (char *)_dst;
        char *src = (char *)_src;

        for (size_t i = 0; i < size; i++)
            *dst++ = *src++;
    }
}

#include <iostream>

std::string ADASIS::ADASIS_utils::ADASIS_Parsing(const std::string& json_file, char* data) {
    //std::ifstream file(json_file);
    if (!nlohmann::json::accept(std::move(json_file))) {
        std::cout << "[FAILED]Json not valid" << std::endl;
        return "";
    }

    tp::s32 canId = 0;

    nlohmann::json _json = nlohmann::json::parse(std::move(json_file));

    canId = _json.at("msgType");

    if (canId == 100) {
        this->dataSendIntervalMilliseconds = (_json.contains("dataSendInterval")) ? (int)_json.at("dataSendInterval") : 0;
    }

    if (canId == 1) {
        ADASIS_position ADASIS;

        ADASIS.msgType = 1;
        ADASIS.cyclicCounter = (_json.contains("cyclicCounter")) ? (int)_json.at("cyclicCounter") : 0;
        ADASIS.pathIndex = (_json.contains("pathIndex")) ? (int)_json.at("pathIndex") : 0;
        ADASIS.offset = (_json.contains("offset")) ? (int)_json.at("offset") : 0;
        ADASIS.positionIndex = (_json.contains("positionIndex")) ? (int)_json.at("positionIndex") : 0;
        ADASIS.positionAge = (_json.contains("positionAge")) ? (int)_json.at("positionAge") : 0;
        ADASIS.speed = (_json.contains("speed")) ? (int)_json.at("speed") : 0;
        ADASIS.relativeHeading = (_json.contains("relativeHeading")) ? (int)_json.at("relativeHeading") : 0;
        ADASIS.positionProbability = (_json.contains("positionProbability")) ? (int)_json.at("positionProbability") : 0;
        ADASIS.positionConfidence = (_json.contains("positionConfidence")) ? (int)_json.at("positionConfidence") : 0;
        ADASIS.currentLane = (_json.contains("currentLane")) ? (int)_json.at("currentLane") : 0;

        data[0] = ((ADASIS.msgType << 5) | (ADASIS.offset >> 8));
        data[1] = (char)ADASIS.offset;
        data[2] = ((ADASIS.cyclicCounter << 6) | ADASIS.pathIndex);
        data[3] = ((ADASIS.positionIndex << 6) | (ADASIS.positionProbability << 1) | (ADASIS.positionAge >> 8));
        data[4] = (char)ADASIS.positionAge;
        data[5] = 0b00000000;
        data[5] = ((ADASIS.positionConfidence << 4) | (ADASIS.currentLane << 1) | (ADASIS.speed >> 8));
        data[6] = (char)ADASIS.speed;
        data[7] = (char)ADASIS.relativeHeading;

        std::cout << "sended position: " << std::endl;
    }

    if (canId == 2) {
        ADASIS_segment ADASIS;

        ADASIS.msgType = 2;
        ADASIS.cyclicCounter = (_json.contains("cyclicCounter")) ? (int)_json.at("cyclicCounter") : 0;
        ADASIS.pathIndex = (_json.contains("pathIndex")) ? (int)_json.at("pathIndex") : 0;
        ADASIS.retransmission = (_json.contains("retransmission")) ? (int)_json.at("retransmission") : 0;
        ADASIS.offset = (_json.contains("offset")) ? (int)_json.at("offset") : 0;
        ADASIS.update = (_json.contains("update")) ? (int)_json.at("update") : 0;
        ADASIS.functionalRoadClass = (_json.contains("functionalRoadClass")) ? (int)_json.at("functionalRoadClass") : 0;
        ADASIS.formOfWay = (_json.contains("formOfWay")) ? (int)_json.at("formOfWay") : 0;
        ADASIS.effectiveSpeedLimit = (_json.contains("effectiveSpeedLimit")) ? (int)_json.at("effectiveSpeedLimit") : 0;
        ADASIS.effectiveSpeedLimitType = (_json.contains("effectiveSpeedLimitType")) ? (int)_json.at("effectiveSpeedLimitType") : 0;
        ADASIS.numberOfIanesInDrivingDirection = (_json.contains("numberOfLanesInDrivingDirection")) ? (int)_json.at("numberOfLanesInDrivingDirection") : 0;
        ADASIS.numberOfIanesInOppositeDirection = (_json.contains("numberOfLanesInOppositeDirection")) ? (int)_json.at("numberOfLanesInOppositeDirection") : 0;
        ADASIS.tunnel = (_json.contains("tunnel")) ? (int)_json.at("tunnel") : 0;
        ADASIS.bridge = (_json.contains("bridge")) ? (int)_json.at("bridge") : 0;
        ADASIS.dividedRoad = (_json.contains("dividedRoad")) ? (int)_json.at("dividedRoad") : 0;
        ADASIS.builtUpArea = (_json.contains("builtUpArea")) ? (int)_json.at("builtUpArea") : 0;
        ADASIS.complexIntersection = (_json.contains("complexIntersection")) ? (int)_json.at("complexIntersection") : 0;
        ADASIS.relativeProbability = (_json.contains("relativeProbability")) ? (int)_json.at("relativeProbability") : 0;
        ADASIS.partOfCalculateRoute = (_json.contains("partOfCalculatedRoute")) ? (int)_json.at("partOfCalculatedRoute") : 0;

        data[0] = ((ADASIS.msgType << 5) | (ADASIS.offset >> 8));
        data[1] = (char)ADASIS.offset;
        data[2] = ((ADASIS.cyclicCounter << 6) | ADASIS.pathIndex);
        data[3] = ((ADASIS.tunnel << 6) | (ADASIS.bridge << 4) | (ADASIS.builtUpArea << 2) | (ADASIS.retransmission << 1) | ADASIS.update);
        data[4] = ((ADASIS.relativeProbability << 3) | ADASIS.functionalRoadClass);
        data[5] = ((ADASIS.partOfCalculateRoute << 6) | (ADASIS.complexIntersection << 4) | ADASIS.formOfWay);
        data[6] = ((ADASIS.effectiveSpeedLimit << 3) | ADASIS.effectiveSpeedLimitType);
        data[7] = 0b00000000;
        data[7] = ((ADASIS.dividedRoad << 5) | (ADASIS.numberOfIanesInOppositeDirection << 3) | ADASIS.numberOfIanesInDrivingDirection);

        std::cout << "sended segment: " << std::endl;
    }

    if (canId == 3) {
        ADASIS_stub ADASIS;

        ADASIS.msgType = 3;
        ADASIS.cyclicCounter = (_json.contains("cyclicCounter")) ? (int)_json.at("cyclicCounter") : 0;
        ADASIS.pathIndex = (_json.contains("pathIndex")) ? (int)_json.at("pathIndex") : 0;
        ADASIS.retransmission = (_json.contains("retransmission")) ? (int)_json.at("retransmission") : 0;
        ADASIS.offset = (_json.contains("offset")) ? (int)_json.at("offset") : 0;
        ADASIS.update = (_json.contains("update")) ? (int)_json.at("update") : 0;
        ADASIS.subPathIndex = (_json.contains("subPathIndex")) ? (int)_json.at("subPathIndex") : 0;
        ADASIS.turnAngle = (_json.contains("turnAngle")) ? (int)_json.at("turnAngle") : 0;
        ADASIS.relativeProbability = (_json.contains("relativeProbability")) ? (int)_json.at("relativeProbability") : 0;
        ADASIS.functionalRoadClass = (_json.contains("functionalRoadClass")) ? (int)_json.at("functionalRoadClass") : 0;
        ADASIS.formOfWay = (_json.contains("formOfWay")) ? (int)_json.at("formOfWay") : 0;
        ADASIS.numberOfIanesInDrivingDirection = (_json.contains("numberOfLanesInDrivingDirection")) ? (int)_json.at("numberOfLanesInDrivingDirection") : 0;
        ADASIS.numberOfIanesInOppositeDirection = (_json.contains("numberOfLanesInOppositeDirection")) ? (int)_json.at("numberOfLanesInOppositeDirection") : 0;
        ADASIS.complexIntersection = (_json.contains("complexIntersection")) ? (int)_json.at("complexIntersection") : 0;
        ADASIS.rightOfWay = (_json.contains("rightOfWay")) ? (int)_json.at("rightOfWay") : 0;
        ADASIS.partOfCalculateRoute = (_json.contains("partOfCalculatedRoute")) ? (int)_json.at("partOfCalculatedRoute") : 0;
        ADASIS.lastStubAtOffset = (_json.contains("lastStubAtOffset")) ? (int)_json.at("lastStubAtOffset") : 0;

        data[0] = ((ADASIS.msgType << 5) | (ADASIS.offset >> 8));
        data[1] = (char)ADASIS.offset;
        data[2] = ((ADASIS.cyclicCounter << 6) | ADASIS.pathIndex);
        data[3] = ((ADASIS.subPathIndex << 2) | (ADASIS.retransmission << 1) | ADASIS.update);
        data[4] = ((ADASIS.relativeProbability << 3) | ADASIS.functionalRoadClass );
        data[5] = ((ADASIS.partOfCalculateRoute << 6) | (ADASIS.complexIntersection << 4) | ADASIS.formOfWay);
        data[6] = (char)ADASIS.turnAngle;
        data[7] = ((ADASIS.lastStubAtOffset << 7) | (ADASIS.rightOfWay << 5) | (ADASIS.numberOfIanesInOppositeDirection << 3) | ADASIS.numberOfIanesInDrivingDirection);
    
        std::cout << "sended stub: " << std::endl;
    }

    if (canId == 4) {
        ADASIS_profshort ADASIS;

        ADASIS.msgType = 4;
        ADASIS.cyclicCounter = (_json.contains("cyclicCounter")) ? (int)_json.at("cyclicCounter") : 0;
        ADASIS.pathIndex = (_json.contains("pathIndex")) ? (int)_json.at("pathIndex") : 0;
        ADASIS.retransmission = (_json.contains("retransmission")) ? (int)_json.at("retransmission") : 0;
        ADASIS.accuracy = (_json.contains("accuracy")) ? (int)_json.at("accuracy") : 0;
        ADASIS.controlPoint = (_json.contains("controlPoint")) ? (int)_json.at("controlPoint") : 0;
        ADASIS.offset = (_json.contains("offset")) ? (int)_json.at("offset") : 0;
        ADASIS.profileType = (_json.contains("profileType")) ? (int)_json.at("profileType") : 0;
        ADASIS.distance1 = (_json.contains("distance1")) ? (int)_json.at("distance1") : 0;
        ADASIS.update = (_json.contains("update")) ? (int)_json.at("update") : 0;
        ADASIS.value0 = (_json.contains("value0")) ? (int)_json.at("value0") : 0;
        ADASIS.value1 = (_json.contains("value1")) ? (int)_json.at("value1") : 0;

        data[0] = ((ADASIS.msgType << 5) | (ADASIS.offset >> 8));
        data[1] = (char)ADASIS.offset;
        data[2] = ((ADASIS.cyclicCounter << 6) | ADASIS.pathIndex);
        data[3] = ((ADASIS.profileType << 3) | (ADASIS.controlPoint << 2) | (ADASIS.retransmission << 1) | ADASIS.update);
        data[4] = ((ADASIS.accuracy << 6) | (ADASIS.distance1 >> 4));
        data[5] = ((ADASIS.distance1 << 4) | (ADASIS.value0 >> 6));
        data[6] = ((ADASIS.value0 << 2) | (ADASIS.value1 >> 8));
        data[7] = (char)(ADASIS.value1);

        std::cout << "sended profshort: " << std::endl;
    }

    if (canId == 5) {
        ADASIS_proflong ADASIS;

        ADASIS.msgType = 5;
        ADASIS.offset = (_json.contains("offset")) ? (int)_json.at("offset") : 0;
        ADASIS.cyclicCounter = (_json.contains("cyclicCounter")) ? (int)_json.at("cyclicCounter") : 0;
        ADASIS.pathIndex = (_json.contains("pathIndex")) ? (int)_json.at("pathIndex") : 0;
        ADASIS.profileType = (_json.contains("profileType")) ? (int)_json.at("profileType") : 0;
        ADASIS.controlPoint = (_json.contains("controlPoint")) ? (int)_json.at("controlPoint") : 0;
        ADASIS.retransmission = (_json.contains("retransmission")) ? (int)_json.at("retransmission") : 0;
        ADASIS.update = (_json.contains("update")) ? (int)_json.at("update") : 0;
        ADASIS.value = (_json.contains("value")) ? (int)_json.at("value") : 0;

        

        data[0] = ((ADASIS.msgType << 5) | (ADASIS.offset >> 8));
        data[1] = (char)ADASIS.offset;
        data[2] = ((ADASIS.cyclicCounter << 6) | ADASIS.pathIndex);
        data[3] = 
        (((ADASIS.profileType << 3) & 0b11111000) | 
        ((ADASIS.controlPoint << 2) & 0b00000100) |
         ((ADASIS.retransmission << 1) & 0b00000010) | 
         (ADASIS.update & 0b00000001));
        data[4] = (char)((ADASIS.value >> 24) & 0xFF);
        data[5] = (char)((ADASIS.value >> 16) & 0xFF);
        data[6] = (char)((ADASIS.value >> 8) & 0xFF);
        data[7] = (char)(ADASIS.value & 0xFF);

        std::cout << "sended proflong: " << std::endl;
        std::cout << "value:" << ADASIS.value << std::endl;
    }

    if (canId == 6) {
        ADASIS_metadata ADASIS;

        ADASIS.msgType = 6;
        ADASIS.mapProvider = (_json.contains("mapProvider")) ? (int)_json.at("mapProvider") : 0;
        ADASIS.countryCode = (_json.contains("countryCode")) ? (int)_json.at("countryCode") : 0;
        ADASIS.cyclicCounter = (_json.contains("cyclicCounter")) ? (int)_json.at("cyclicCounter") : 0;
        ADASIS.majorProtocolVersion = (_json.contains("majorProtocolVersion")) ? (int)_json.at("majorProtocolVersion") : 0;
        ADASIS.minorProtocolSubVersion = (_json.contains("minorProtocolSubVersion")) ? (int)_json.at("minorProtocolSubVersion") : 0;
        ADASIS.hardwareVersion = (_json.contains("hardwareVersion")) ? (int)_json.at("hardwareVersion") : 0;
        ADASIS.drivingSide = (_json.contains("drivingSide")) ? (int)_json.at("drivingSide") : 0;
        ADASIS.regionCode = (_json.contains("regionCode")) ? (int)_json.at("regionCode") : 0;
        ADASIS.mapVersionYearQuarter = (_json.contains("mapVersionYearQuarter")) ? (int)_json.at("mapVersionYearQuarter") : 0;
        ADASIS.mapVersionYear = (_json.contains("mapVersionYear")) ? (int)_json.at("mapVersionYear") : 0;
        ADASIS.speedUnits = (_json.contains("speedUnits")) ? (int)_json.at("speedUnits") : 0;
        ADASIS.minorProtocolVersion = (_json.contains("minorProtocolVersion")) ? (int)_json.at("minorProtocolVersion") : 0;

        data[0] = ((ADASIS.msgType << 5) | (ADASIS.mapProvider << 2) | (ADASIS.countryCode >> 8));
        data[1] = (char)ADASIS.countryCode;
        data[2] = ((ADASIS.cyclicCounter << 6) | (ADASIS.majorProtocolVersion << 4) | (ADASIS.minorProtocolSubVersion << 1) | (ADASIS.hardwareVersion >> 8));
        data[3] = (char)ADASIS.hardwareVersion;
        data[4] = ((ADASIS.drivingSide << 7) | (ADASIS.regionCode >> 8));
        data[5] = (char)ADASIS.regionCode;
        data[6] = ((ADASIS.mapVersionYearQuarter << 6) | ADASIS.mapVersionYear);
        data[7] = 0b00000000;
        data[7] = ((ADASIS.speedUnits << 4) | ADASIS.minorProtocolVersion);

        std::cout << "sended metadata: " << std::endl;
    }

    return "none";
}