ADD_EXECUTABLE(atomese_representation
        atomese_representation)

TARGET_LINK_LIBRARIES (atomese_representation
        comboreduct
        ${COGUTIL_LIBRARY}
        ${ATOMSPACE_atombase_LIBRARY}
        )
