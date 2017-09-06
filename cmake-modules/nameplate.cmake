include(${CMAKE_CURRENT_LIST_DIR}/GetGitRevisionDescription.cmake)

get_git_head_revision(GIT_BRANCH GIT_REVISION)
git_local_changes(GIT_LOCAL_CHANGES)
string(SUBSTRING ${GIT_REVISION} 0 6 GIT_REVISION_SHORT)
add_definitions(-DGIT_REVISION=\"${GIT_REVISION_SHORT}\")
add_definitions(-DGIT_CHANGES=\"${GIT_LOCAL_CHANGES}\")
message("Revision ${GIT_REVISION_SHORT} ${GIT_LOCAL_CHANGES}")