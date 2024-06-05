#ifndef ARCHETYPE_H
#define ARCHETYPE_H

#include <vector>

class Archetype {
private:
    std::vector<int> entityIDs;
        unsigned int id;

        public:
            Archetype(unsigned int archetypeId) : id(archetypeId) {}

                unsigned int getId() const {
                        return id;
                            }

                                const std::vector<int>& getEntityIDs() const {
                                        return entityIDs;
                                            }

                                                void setEntityIDs(const std::vector<int>& newEntityIDs) {
                                                        entityIDs = newEntityIDs;
                 
                                                            }
                                                            };

                                                            #endif // ARCHETYPE_H