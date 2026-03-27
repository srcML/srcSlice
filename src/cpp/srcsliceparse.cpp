// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcsliceparse.cpp
 *
 * @copyright Copyright (C) 2018-2024 srcML, LLC. (www.srcML.org)
 *
 * This file is part of the srcSlice application.
 */

#include <srcsliceparse.hpp>

// Returns a numeric id based on the type of C++ IO operation is within given NameData
// -1 => None | 0 => cout | 1 => cerr | 2 => cin
int ExprParse::IsIO(const SliceCtx& sctx, const srcDispatch::NameData* nameDataPtr) {
    if (nameDataPtr == nullptr || sctx.currentFileLanguage != "C++") {
        return -1;
    }

    // check for straight up simple call
    if (nameDataPtr->name == "cout") return 0;
    else if (nameDataPtr->name == "cerr") return 1;
    else if (nameDataPtr->name == "cin") return 2;

    // check for scoped-call
    for (const auto& nameElem : nameDataPtr->names) {
        if (nameElem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::NameData>)) {
            std::shared_ptr<srcDispatch::NameData> nameData = std::any_cast<std::shared_ptr<srcDispatch::NameData>>(nameElem.GetElement());
            
            if (nameData->name.ToString() == "cout") return 0;
            else if (nameData->name.ToString() == "cerr") return 1;
            else if (nameData->name.ToString() == "cin") return 2;
        }
    }

    return -1;
};

/**
 * Find the root name within a complex name
 * 
 * arr[2] => arr
 * obj_ptr->attribute => obj_ptr
 * obj.name => obj
 */

std::string ExprParse::FindName(const std::vector<srcDispatch::DeltaElement<std::any>>& names, SlicePosition& namePos) {
    for (const auto& nameElem : names) {
        if (nameElem.GetElement().type() == typeid(std::shared_ptr<srcDispatch::NameData>)) {
            auto nameData = std::any_cast<std::shared_ptr<srcDispatch::NameData>>(nameElem.GetElement());
            if (nameData->name.ToString().empty()) continue;

            namePos = SlicePosition(
                nameData->startPosition,
                nameData->endPosition,
                namePos.GetFileName()
            );
            return nameData->name.ToString();
        }
    }
    return "";
};

bool ExprParse::checkPosition(SlicePosition& a, SlicePosition& b) {
    if (a.GetStart()->GetLine() < b.GetStart()->GetLine()) return true;
    if (a.GetStart()->GetLine() > b.GetStart()->GetLine()) return false;
    
    // when lines match, check column
    if (a.GetStart()->GetColumn() < b.GetStart()->GetColumn()) return true;
    if (a.GetStart()->GetColumn() > b.GetStart()->GetColumn()) return false;

    return false;
}

// capture this to access the member variable profileMap to avoid a capture-all
void ExprParse::pushDvar(SliceProfileMap& profileMap, std::string lhsName, std::string rhsName, ExprParse::ExprCtx& ectx) {
    if (lhsName == rhsName) return;

    // LHS is data dependent of RHS => insert LHS as dvar of RHS slice
    auto rhsSpi = profileMap.find(rhsName);
    if (rhsSpi != profileMap.end()) {
        SliceProfile* sp = &(rhsSpi->second.back());

        // a position cannot occur before the position of the initial
        // unless its from a function call
        if (checkPosition(ectx.namePos, sp->declPosition) || ectx.containedClass != sp->nameOfContainingClass) {
            // search backwards from second back to find a valid profile
            for (auto rSpi = rhsSpi->second.rbegin(); rSpi != rhsSpi->second.rend(); ++rSpi) {
                // if the pos occurs after the initial => valid profile
                if (checkPosition(rSpi->declPosition, ectx.namePos) && ectx.containedClass == rSpi->nameOfContainingClass) {
                    sp = &(*rSpi);
                    break;
                }
            }
        }

        sp->insertDvar(lhsName, ectx.namePos);
    }
};

void ExprParse::pushAlias(SliceProfileMap& profileMap, std::string lhsName, std::string rhsName, ExprParse::ExprCtx& ectx) {
    if (lhsName == rhsName) return;

    // LHS is an alias to RHS => insert RHS as alias of LHS
    auto lhsSpi = profileMap.find(lhsName);
    if (lhsSpi != profileMap.end()) {
        SliceProfile* sp = &(lhsSpi->second.back());

        // a position cannot occur before the position of the initial
        // unless its from a function call
        if (checkPosition(ectx.namePos, sp->declPosition) || ectx.containedClass != sp->nameOfContainingClass) {
            // search backwards from second back to find a valid profile
            for (auto rSpi = lhsSpi->second.rbegin(); rSpi != lhsSpi->second.rend(); ++rSpi) {
                // if the pos occurs after the initial => valid profile
                if (checkPosition(rSpi->declPosition, ectx.namePos) && ectx.containedClass == rSpi->nameOfContainingClass) {
                    sp = &(*rSpi);
                    break;
                }
            }
        }

        sp->insertAlias(rhsName, ectx.namePos);
        // mark the pointers current reference
        sp->currentPointerReference = rhsName;
    }
};

void ExprParse::pushUse(SliceProfileMap& profileMap, SliceProfileIterator spi, ExprParse::ExprCtx& ectx) {
    if (spi == profileMap.end()) return;
    SliceProfile* sp = &(spi->second.back());

    // a position cannot occur before the position of the initial
    // unless its from a function call
    if (checkPosition(ectx.namePos, sp->declPosition) || ectx.containedClass != sp->nameOfContainingClass) {
        // search backwards from second back to find a valid profile
        for (auto rSpi = spi->second.rbegin(); rSpi != spi->second.rend(); ++rSpi) {
            // if the pos occurs after the initial => valid profile
            if (checkPosition(rSpi->declPosition, ectx.namePos) && ectx.containedClass == rSpi->nameOfContainingClass) {
                sp = &(*rSpi);
                break;
            }
        }
    }

    sp->uses.insert(ectx.namePos);

    // if sp is a pointer type we need to show the use across its reference chain
    if (sp->isPointer) {
        std::vector<std::string> visited; // ensure we do not enter circular dependence
        std::string referenceName = sp->currentPointerReference;
        auto aspi = profileMap.find(referenceName);
        
        while (true) {
            if (aspi == profileMap.end()) break;
            if (std::find(visited.begin(), visited.end(), referenceName) != visited.end()) break;

            aspi->second.back().uses.insert(ectx.namePos);

            visited.push_back(referenceName);

            referenceName = aspi->second.back().currentPointerReference;
            aspi = profileMap.find(referenceName);
        }
    }
};

void ExprParse::popUse(SliceProfileMap& profileMap, SliceProfileIterator spi, ExprParse::ExprCtx& ectx) {
    if (spi == profileMap.end()) return;
    SliceProfile* sp = &(spi->second.back());

    if (checkPosition(ectx.namePos, sp->declPosition) || ectx.containedClass != sp->nameOfContainingClass) {
        for (auto rSpi = spi->second.rbegin(); rSpi != spi->second.rend(); ++rSpi) {
            if (checkPosition(rSpi->declPosition, ectx.namePos) && ectx.containedClass == rSpi->nameOfContainingClass) {
                sp = &(*rSpi);
                break;
            }
        }
    }

    sp->uses.erase(ectx.namePos);
};

void ExprParse::pushDef(SliceProfileMap& profileMap, SliceProfileIterator spi, ExprParse::ExprCtx& ectx) {
    if (spi == profileMap.end()) return;
    SliceProfile* sp = &(spi->second.back());

    if (checkPosition(ectx.namePos, sp->declPosition) || ectx.containedClass != sp->nameOfContainingClass) {
        for (auto rSpi = spi->second.rbegin(); rSpi != spi->second.rend(); ++rSpi) {
            if (checkPosition(rSpi->declPosition, ectx.namePos) && ectx.containedClass == rSpi->nameOfContainingClass) {
                sp = &(*rSpi);
                break;
            }
        }
    }

    sp->definitions.insert(ectx.namePos);
};
void ExprParse::popDef(SliceProfileMap& profileMap, SliceProfileIterator spi, ExprParse::ExprCtx& ectx) {
    if (spi == profileMap.end()) return;
    SliceProfile* sp = &(spi->second.back());

    if (checkPosition(ectx.namePos, sp->declPosition) || ectx.containedClass != sp->nameOfContainingClass) {
        for (auto rSpi = spi->second.rbegin(); rSpi != spi->second.rend(); ++rSpi) {
            if (checkPosition(rSpi->declPosition, ectx.namePos) && ectx.containedClass == rSpi->nameOfContainingClass) {
                sp = &(*rSpi);
                break;
            }
        }
    }

    sp->definitions.erase(ectx.namePos);
};

void ExprParse::updateLHS(SliceProfileMap& profileMap, ExprParse::ExprCtx& ectx, const std::string& target, int i, int L) {
    // exit if there is no slice of the target
    if (profileMap.find(target) == profileMap.end()) return;
    if (ectx.lhsStackPtr == nullptr) return;

    std::vector<std::string>& lhsStack = *(ectx.lhsStackPtr);
    for (const std::string& lhsName : lhsStack) {
        // insert dvars and or aliases
        if (ectx.addressOf) {
            if (i == -1 || i+1 == L) {
                ExprParse::pushAlias(profileMap, lhsName, target, ectx);
                ectx.addressOf = false;
            }
        } else {
            ExprParse::pushDvar(profileMap, lhsName, target, ectx);
        }
    }
};