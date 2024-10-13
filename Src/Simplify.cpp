#include "Simplify.hpp"
#include <memory>
#include <queue>
#include <vector>

std::map<Vec3, Vertex> CreateVertices (Mesh const &input) {
    std::map<Vec3, Vertex> vectorVertex;
    for (const Triangle &t : input) {
        vectorVertex[t.v1] = Vertex (t.v1);
        vectorVertex[t.v2] = Vertex (t.v2);
        vectorVertex[t.v3] = Vertex (t.v3);
    }

    // Accumulate quadric matrices for each vertex based on its faces
    for (const Triangle &t : input) {
        Matrix q = t.Quadric ();
        Vertex &v1 = vectorVertex[t.v1];
        Vertex &v2 = vectorVertex[t.v2];
        Vertex &v3 = vectorVertex[t.v3];
        v1.q = v1.q + q;
        v2.q = v2.q + q;
        v3.q = v3.q + q;
    }

    return vectorVertex;
}

std::map<Vertex const *, std::vector<Face const *>> CreateFacesAndMapVertices (
    Mesh const &input,
    std::map<Vec3, Vertex> const &vectorVertex,
    std::vector<std::unique_ptr<Face>> &facesToDelete) {

    std::map<Vertex const *, std::vector<Face const *>> vertexFaces;

    for (const Triangle &t : input) {
        Vertex const &v1 = vectorVertex.at (t.v1);
        Vertex const &v2 = vectorVertex.at (t.v2);
        Vertex const &v3 = vectorVertex.at (t.v3);
        facesToDelete.push_back (std::make_unique<Face> (&v1, &v2, &v3));
        Face *f = facesToDelete.back ().get ();

        vertexFaces[&v1].push_back (f);
        vertexFaces[&v2].push_back (f);
        vertexFaces[&v3].push_back (f);
    }

    return vertexFaces;
}

Mesh Simplify (Mesh const &input, double factor) {
    std::vector<std::unique_ptr<Vertex>> vertexesToDelete;
    std::vector<std::unique_ptr<Face>> facesToDelete;
    std::vector<std::unique_ptr<Edge>> newEdges;

    size_t numFaces = input.size ();

    auto const vectorVertex = CreateVertices (input);
    auto vertexFaces = CreateFacesAndMapVertices (input, vectorVertex, facesToDelete);

    // Find distinct pairs (Edges)
    std::map<PairKey, Edge> edges;
    for (const Triangle &t : input) {
        Vertex const *v1 = &vectorVertex.at (t.v1);
        Vertex const *v2 = &vectorVertex.at (t.v2);
        Vertex const *v3 = &vectorVertex.at (t.v3);

        edges[MakePairKey (v1, v2)] = Edge (v1, v2);
        edges[MakePairKey (v2, v3)] = Edge (v2, v3);
        edges[MakePairKey (v3, v1)] = Edge (v3, v1);
    }

    // assume edges size doesn't change

    // Enqueue edges and map vertex => edges
    std::priority_queue<Edge *, std::vector<Edge *>, EdgeComparator> queue;
    std::map<Vertex const *, std::vector<Edge *>> vertexEdges;
    for (auto &p : edges) {
        queue.push (&p.second);
        vertexEdges[p.second.A].push_back (&p.second);
        vertexEdges[p.second.B].push_back (&p.second);
    }

    // Simplify
    size_t target = static_cast<int> (numFaces * factor);
    while (numFaces > target && queue.size () > 0) {
        Edge *p = queue.top ();
        queue.pop ();

        if (p->Removed) {
            continue;
        }
        p->Removed = true;

        // Get related faces
        std::set<Face const *> distinctFaces;
        for (Face const *f : vertexFaces[p->A]) {
            if (!f->Removed) {
                distinctFaces.insert (f);
            }
        }
        for (Face const *f : vertexFaces[p->B]) {
            if (!f->Removed) {
                distinctFaces.insert (f);
            }
        }

        // Get related edges
        std::unordered_set<Edge *> distinctEdges;
        for (const auto &q : vertexEdges[p->A]) {
            if (!q->Removed) {
                distinctEdges.insert (q);
            }
        }
        for (const auto &q : vertexEdges[p->B]) {
            if (!q->Removed) {
                distinctEdges.insert (q);
            }
        }

        // Create the new vertex
        vertexesToDelete.push_back (p->ComputeNewVertex ());
        Vertex *v = vertexesToDelete.back ().get ();

        // Update faces
        std::vector<Face *> newFaces;
        bool valid = true;
        for (const auto &f : distinctFaces) {
            Vertex const *v1 = f->v1;
            Vertex const *v2 = f->v2;
            Vertex const *v3 = f->v3;
            if (v1 == p->A || v1 == p->B) {
                v1 = v;
            }
            if (v2 == p->A || v2 == p->B) {
                v2 = v;
            }
            if (v3 == p->A || v3 == p->B) {
                v3 = v;
            }
            facesToDelete.push_back (std::make_unique<Face> (v1, v2, v3));
            Face *face = facesToDelete.back ().get ();
            if (face->Degenerate ()) {
                continue;
            }
            if (face->Normal ().Dot (f->Normal ()) < EPSILON) {
                valid = false;
                break;
            }
            newFaces.push_back (face);
        }
        if (!valid) {
            continue;
        }
        vertexFaces.erase (p->A);
        vertexFaces.erase (p->B);
        for (Face const *f : distinctFaces) {
            f->Removed = true;
            numFaces--;
        }
        for (Face const *f : newFaces) {
            numFaces++;
            vertexFaces[f->v1].push_back (f);
            vertexFaces[f->v2].push_back (f);
            vertexFaces[f->v3].push_back (f);
        }

        // Update edges and prune current pair
        vertexEdges.erase (p->A);
        vertexEdges.erase (p->B);
        std::set<Vec3> seen;
        for (Edge *q : distinctEdges) {
            q->Removed = true;
            queue.push (q);
            Vertex const *a = q->A;
            Vertex const *b = q->B;
            if (a == p->A || a == p->B) {
                a = v;
            }
            if (b == p->A || b == p->B) {
                b = v;
            }
            if (b == v) {
                std::swap (a, b);
            }
            if (seen.count (b->v)) {
                continue;
            }
            seen.insert (b->v);

            newEdges.push_back (std::make_unique<Edge> (a, b));
            Edge *q2 = newEdges.back ().get ();
            queue.push (q2);
            vertexEdges[a].push_back (q2);
            vertexEdges[b].push_back (q2);
        }
    }

    return ConstructMesh (vertexFaces);
}

Mesh ConstructMesh (std::map<Vertex const *, std::vector<Face const *>> const &vertexFaces) {
    // Find distinct faces
    std::unordered_set<Face const *> distinctFaces;
    distinctFaces.reserve (vertexFaces.size ());
    for (auto &faces : vertexFaces) {
        for (Face const *f : faces.second) {
            if (!f->Removed) {
                distinctFaces.emplace (f);
            }
        }
    }

    // Construct resulting mesh
    std::vector<Triangle> simplifiedMesh;
    simplifiedMesh.reserve (distinctFaces.size ());

    for (Face const *f : distinctFaces) {
        simplifiedMesh.emplace_back (f->v1->v, f->v2->v, f->v3->v);
    }
    return simplifiedMesh;
}
