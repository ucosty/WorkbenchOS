// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include "Error.h"

namespace EFI {
Std::StringView error_to_string(ErrorType type) {
    switch (type) {
        case EFI::ErrorType::LoadError:
            return "EFI_LOAD_ERROR"_sv;
        case EFI::ErrorType::InvalidParameter:
            return "EFI_INVALID_PARAMETER"_sv;
        case EFI::ErrorType::Unsupported:
            return "EFI_UNSUPPORTED"_sv;
        case EFI::ErrorType::BadBufferSize:
            return "EFI_BAD_BUFFER_SIZE"_sv;
        case EFI::ErrorType::BufferTooSmall:
            return "EFI_BUFFER_TOO_SMALL"_sv;
        case EFI::ErrorType::NotReady:
            return "EFI_NOT_READY"_sv;
        case EFI::ErrorType::DeviceError:
            return "EFI_DEVICE_ERROR"_sv;
        case EFI::ErrorType::WriteProtected:
            return "EFI_WRITE_PROTECTED"_sv;
        case EFI::ErrorType::OutOfResources:
            return "EFI_OUT_OF_RESOURCES"_sv;
        case EFI::ErrorType::VolumeCorrupted:
            return "EFI_VOLUME_CORRUPTED"_sv;
        case EFI::ErrorType::VolumeFull:
            return "EFI_VOLUME_FULL"_sv;
        case EFI::ErrorType::NoMedia:
            return "EFI_NO_MEDIA"_sv;
        case EFI::ErrorType::MediaChanged:
            return "EFI_MEDIA_CHANGED"_sv;
        case EFI::ErrorType::NotFound:
            return "EFI_NOT_FOUND"_sv;
        case EFI::ErrorType::AccessDenied:
            return "EFI_ACCESS_DENIED"_sv;
        case EFI::ErrorType::NoResponse:
            return "EFI_NO_RESPONSE"_sv;
        case EFI::ErrorType::NoMapping:
            return "EFI_NO_MAPPING"_sv;
        case EFI::ErrorType::Timeout:
            return "EFI_TIMEOUT"_sv;
        case EFI::ErrorType::NotStarted:
            return "EFI_NOT_STARTED"_sv;
        case EFI::ErrorType::AlreadyStarted:
            return "EFI_ALREADY_STARTED"_sv;
        case EFI::ErrorType::Aborted:
            return "EFI_ABORTED"_sv;
        case EFI::ErrorType::IcmpError:
            return "EFI_ICMP_ERROR"_sv;
        case EFI::ErrorType::TftpError:
            return "EFI_TFTP_ERROR"_sv;
        case EFI::ErrorType::ProtocolError:
            return "EFI_PROTOCOL_ERROR"_sv;
        case EFI::ErrorType::IncompatibleVersion:
            return "EFI_INCOMPATIBLE_VERSION"_sv;
        case EFI::ErrorType::SecurityViolation:
            return "EFI_SECURITY_VIOLATION"_sv;
        case EFI::ErrorType::CrcError:
            return "EFI_CRC_ERROR"_sv;
        case EFI::ErrorType::EndOfMedia:
            return "EFI_END_OF_MEDIA"_sv;
        case EFI::ErrorType::EndOfFile:
            return "EFI_END_OF_FILE"_sv;
        case EFI::ErrorType::InvalidLanguage:
            return "EFI_INVALID_LANGUAGE"_sv;
        case EFI::ErrorType::CompromisedData:
            return "EFI_COMPROMISED_DATA"_sv;
        case EFI::ErrorType::IpAddressConflict:
            return "EFI_IP_ADDRESS_CONFLICT"_sv;
        case EFI::ErrorType::HttpError:
            return "EFI_HTTP_ERROR"_sv;
        case EFI::ErrorType::InvalidMemoryMap:
            return "EFI_INVALID_MEMORY_MAP"_sv;
        default:
            return "EFI_UNKNOWN"_sv;
    }
}
}// namespace EFI
